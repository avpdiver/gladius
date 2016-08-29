//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_LOCK_FREE_SINGLE_QUEUE_H
#define GLADIUS_LOCK_FREE_SINGLE_QUEUE_H

#include <type_traits>
#include <utility>
#include <cassert>
#include <stdexcept>
#include <new>
#include <cstdint>
#include <cstdlib>		// For malloc/free/abort & size_t
#include <chrono>
#include <atomic>

#include "../platform.h"
#include "../math/math_func.h"
#include "../threading/weak_semaphore.h"
#include "../memory/memory_utils.h"

// A single-producer, single-consumer lock-free queue

// https://github.com/cameron314/readerwriterqueue
// Design: Based on a queue-of-queues. The low-level queues are just
// circular buffers with m_front and m_tail indices indicating where the
// m_next element to dequeue is and where the m_next element can be enqueued,
// respectively. Each low-level queue is called a "block". Each block
// wastes exactly one element's worth of space to keep the design simple
// (if m_front == m_tail then the queue is empty, and can't be full).
// The high-level queue is a circular linked list of blocks; again there
// is a m_front and m_tail, but this time they are pointers to the blocks.
// The m_front block is where the m_next element to be dequeued is, provided
// the block is not empty. The back block is where elements are to be
// enqueued, provided the block is not full.
// The producer thread owns all the m_tail indices/pointers. The consumer
// thread owns all the m_front indices/pointers. Both threads read each
// other's variables, but only the owning thread updates them. E.g. After
// the consumer reads the producer's m_tail, the m_tail may change before the
// consumer is done m_dequeuing an object, but the consumer knows the m_tail
// will never go backwards, only forwards.
// If there is no room to push an object, an additional block (of
// equal size to the last block) is added. Blocks are never removed.

#define UNUSED(v) ((void)v)

namespace gladius
{
	namespace core
	{
		namespace collections
		{
			template<typename T, size_t MAX_BLOCK_SIZE = 512>
			class lock_free_2points_queue 
			{
			public:
				// Constructs a queue that can hold max_size elements without further
				// allocations. If more than MAX_BLOCK_SIZE elements are requested,
				// then several blocks of MAX_BLOCK_SIZE each are reserved (including
				// at least one extra buffer block).
				explicit lock_free_2points_queue(size_t max_size = 15)
				{
					assert(max_size > 0);
					assert(MAX_BLOCK_SIZE == math::ceil_to_power_2(MAX_BLOCK_SIZE) && "MAX_BLOCK_SIZE must be a power of 2");
					assert(MAX_BLOCK_SIZE >= 2 && "MAX_BLOCK_SIZE must be at least 2");

					memory_block *first_block = nullptr;

					// We need a spare slot to fit max_size elements in the block
					m_largest_block_size = math::ceil_to_power_2(max_size + 1);
					if (m_largest_block_size > MAX_BLOCK_SIZE * 2)
					{
						// We need a spare block in case the producer is writing to a different block the consumer is reading from, and
						// wants to push the maximum number of elements. We also need a spare element in each block to avoid the ambiguity
						// between m_front == m_tail meaning "empty" and "full".
						// So the effective number of slots that are guaranteed to be usable at any time is the block size - 1 times the
						// number of blocks - 1. Solving for max_size and applying a ceiling to the division gives us (after simplifying):
						size_t initial_block_count = (max_size + MAX_BLOCK_SIZE * 2 - 3) / (MAX_BLOCK_SIZE - 1);
						m_largest_block_size = MAX_BLOCK_SIZE;
						memory_block *last_block = nullptr;

						for (size_t i = 0; i != initial_block_count; ++i)
						{
							auto block = make_block(m_largest_block_size);
							if (block == nullptr)
							{
								abort();
							}

							if (first_block == nullptr)
							{
								first_block = block;
							}
							else
							{
								last_block->m_next = block;
							}

							last_block = block;
							block->m_next = first_block;
						}
					}
					else
					{
						first_block = make_block(m_largest_block_size);
						if (first_block == nullptr)
						{
							abort();
						}
						first_block->m_next = first_block;
					}
					m_front_block = first_block;
					m_tail_block = first_block;

					// Make sure the reader/writer threads will have the initialized memory setup above:
					std::atomic_thread_fence(std::memory_order_seq_cst);
				}

				// Note: The queue should not be accessed concurrently while it's
				// being deleted. It's up to the user to synchronize this.
				~lock_free_2points_queue() 
				{
					// Make sure we get the latest version of all variables from other CPUs:
					std::atomic_thread_fence(std::memory_order_seq_cst);

					// Destroy any remaining objects in queue and free memory
					memory_block *front_block 		= m_front_block;
					memory_block *block 			= front_block;

					do 
					{
						memory_block *next_block 	= block->m_next;
						size_t block_front 			= block->m_front;
						size_t block_tail 			= block->m_tail;

						for (size_t i = block_front; i != block_tail; i = (i + 1) & block->m_size_mask)
						{
							auto element = reinterpret_cast<T *>(block->m_data + i * sizeof(T));
							element->~T();
							(void) element;
						}

						auto raw_block = block->m_raw_this;
						block->~memory_block();
						std::free(raw_block);
						block = next_block;
					}
					while (block != front_block);
				}

			private:
				// Disable copying
				lock_free_2points_queue(lock_free_2points_queue const &) { }

				// Disable assignment
				lock_free_2points_queue &operator=(lock_free_2points_queue const &) { return *this; }

			public:
				// Enqueues a copy of element if there is room in the queue.
				// Returns true if the element was enqueued, false otherwise.
				// Does not allocate memory.
				inline bool try_push(T const &element) 
				{
					return inner_push<CANNOT_ALLOC>(element);
				}

				// Enqueues a moved copy of element if there is room in the queue.
				// Returns true if the element was enqueued, false otherwise.
				// Does not allocate memory.
				inline bool try_push(T &&element) 
				{
					return inner_push<CANNOT_ALLOC>(std::forward<T>(element));
				}

				// Enqueues a copy of element on the queue.
				// Allocates an additional block of memory if needed.
				// Only fails (returns false) if memory allocation fails.
				inline bool push(T const &element) 
				{
					return inner_push<CAN_ALLOC>(element);
				}

				// Enqueues a moved copy of element on the queue.
				// Allocates an additional block of memory if needed.
				// Only fails (returns false) if memory allocation fails.
				inline bool push(T &&element) 
				{
					return inner_push<CAN_ALLOC>(std::forward<T>(element));
				}


				// Attempts to dequeue an element; if the queue is empty,
				// returns false instead. If the queue has at least one element,
				// moves m_front to result using operator=, then returns true.
				template<typename U>
				bool try_pop(U &result)
				{
					// High-level pseudocode:
					// Remember where the m_tail block is
					// If the m_front block has an element in it, dequeue it
					// Else
					//     If m_front block was the m_tail block when we entered the function, return false
					//     Else advance to m_next block and dequeue the item there

					// Note that we have to use the value of the m_tail block from before we check if the m_front
					// block is full or not, in case the m_front block is empty and then, before we check if the
					// m_tail block is at the m_front block or not, the producer fills up the m_front block *and
					// moves on*, which would make us skip a filled block. Seems unlikely, but was consistently
					// reproducible in practice.
					// In order to avoid overhead in the common case, though, we do a double-checked pattern
					// where we have the fast path if the m_front block is not empty, then read the m_tail block,
					// then re-read the m_front block and check if it's not empty again, then check if the m_tail
					// block has advanced.

					memory_block *front_block = m_front_block.load();
					size_t block_tail = front_block->m_local_tail;
					size_t block_front = front_block->m_front.load();

					if (block_front != block_tail || block_front != (front_block->m_local_tail = front_block->m_tail.load()))
					{
						std::atomic_thread_fence(std::memory_order_acquire);

						non_empty_front_block:
						// Front block not empty, dequeue from here
						auto element = reinterpret_cast<T *>(front_block->m_data + block_front * sizeof(T));
						result = std::move(*element);
						element->~T();

						block_front = (block_front + 1) & front_block->m_size_mask;

						std::atomic_thread_fence(std::memory_order_release);
						front_block->m_front = block_front;
					}
					else if (front_block != m_tail_block.load())
					{
						std::atomic_thread_fence(std::memory_order_acquire);

						front_block = m_front_block.load();
						block_tail = front_block->m_local_tail = front_block->m_tail.load();
						block_front = front_block->m_front.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						if (block_front != block_tail) {
							// Oh look, the m_front block isn't empty after all
							goto non_empty_front_block;
						}

						// Front block is empty but there's another block ahead, advance to it
						memory_block *next_block = front_block->m_next;
						// Don't need an acquire fence here since m_next can only ever be set on the m_tail_block,
						// and we're not the m_tail_block, and we did an acquire earlier after reading m_tail_block which
						// ensures m_next is up-to-date on this CPU in case we recently were at m_tail_block.

						size_t next_block_front = next_block->m_front.load();
						size_t next_block_tail = next_block->m_local_tail = next_block->m_tail.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						// Since the m_tail_block is only ever advanced after being written to,
						// we know there's for sure an element to dequeue on it
						assert(next_block_front != next_block_tail);
						UNUSED(next_block_tail);

						// We're done with this block, let the producer use it if it needs
						std::atomic_thread_fence(std::memory_order_release);
						// Expose possibly pending changes to m_front_block->m_front from last dequeue
						m_front_block = front_block = next_block;

						std::atomic_signal_fence(std::memory_order_release);    // Not strictly needed

						auto element = reinterpret_cast<T *>(front_block->m_data + next_block_front * sizeof(T));

						result = std::move(*element);
						element->~T();

						next_block_front = (next_block_front + 1) & front_block->m_size_mask;

						std::atomic_thread_fence(std::memory_order_release);
						front_block->m_front = next_block_front;
					}
					else
					{
						// No elements in current block and no other block to advance to
						return false;
					}

					return true;
				}


				// Returns a pointer to the m_front element in the queue (the one that
				// would be removed m_next by a call to `try_pop` or `pop`). If the
				// queue appears empty at the time the method is called, nullptr is
				// returned instead.
				// Must be called only from the consumer thread.
				T *front()
				{
					// See try_pop() for reasoning

					memory_block *front_block = m_front_block.load();
					size_t block_tail = front_block->m_local_tail;
					size_t block_front = front_block->m_front.load();

					if (block_front != block_tail || block_front != (front_block->m_local_tail = front_block->m_tail.load()))
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						non_empty_front_block:
						return reinterpret_cast<T *>(front_block->m_data + block_front * sizeof(T));
					}
					else if (front_block != m_tail_block.load())
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						front_block = m_front_block.load();
						block_tail = front_block->m_local_tail = front_block->m_tail.load();
						block_front = front_block->m_front.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						if (block_front != block_tail) {
							goto non_empty_front_block;
						}

						memory_block *next_block = front_block->m_next;

						size_t next_block_front = next_block->m_front.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						assert(next_block_front != next_block->m_tail.load());
						return reinterpret_cast<T *>(next_block->m_data + next_block_front * sizeof(T));
					}

					return nullptr;
				}

				// Removes the m_front element from the queue, if any, without returning it.
				// Returns true on success, or false if the queue appeared empty at the time
				// `pop` was called.
				bool pop()
				{
					// See try_pop() for reasoning

					memory_block *front_block = m_front_block.load();
					size_t block_tail = front_block->m_local_tail;
					size_t block_front = front_block->m_front.load();

					if (block_front != block_tail || block_front != (front_block->m_local_tail = front_block->m_tail.load()))
					{
						std::atomic_thread_fence(std::memory_order_acquire);

						non_empty_front_block:
						auto element = reinterpret_cast<T *>(front_block->m_data + block_front * sizeof(T));
						element->~T();

						block_front = (block_front + 1) & front_block->m_size_mask;

						std::atomic_thread_fence(std::memory_order_release);
						front_block->m_front = block_front;
					}
					else if (front_block != m_tail_block.load())
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						front_block = m_front_block.load();
						block_tail = front_block->m_local_tail = front_block->m_tail.load();
						block_front = front_block->m_front.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						if (block_front != block_tail) {
							goto non_empty_front_block;
						}

						// Front block is empty but there's another block ahead, advance to it
						memory_block *next_block = front_block->m_next;

						size_t next_block_front = next_block->m_front.load();
						size_t next_block_tail = next_block->m_local_tail = next_block->m_tail.load();
						std::atomic_thread_fence(std::memory_order_acquire);

						assert(next_block_front != next_block_tail);
						UNUSED(next_block_tail);

						std::atomic_thread_fence(std::memory_order_release);
						m_front_block = front_block = next_block;

						std::atomic_signal_fence(std::memory_order_release);

						auto element = reinterpret_cast<T *>(front_block->m_data + next_block_front * sizeof(T));
						element->~T();

						next_block_front = (next_block_front + 1) & front_block->m_size_mask;

						std::atomic_thread_fence(std::memory_order_release);
						front_block->m_front = next_block_front;
					}
					else
					{
						// No elements in current block and no other block to advance to
						return false;
					}

					return true;
				}

				// Returns the approximate number of items currently in the queue.
				// Safe to call from both the producer and consumer threads.
				inline size_t size() const
				{
					size_t result = 0;
					memory_block *front_block = m_front_block.load();
					memory_block *block = front_block;
					do
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						size_t block_front = block->m_front.load();
						size_t block_tail = block->m_tail.load();
						result += (block_tail - block_front) & block->m_size_mask;
						block = block->m_next.load();
					}
					while (block != front_block);
					return result;
				}

			private:
				enum e_allocation_mode
				{
					CAN_ALLOC, CANNOT_ALLOC
				};

				template<e_allocation_mode canAlloc, typename U>
				bool inner_push(U &&element)
				{
					// High-level pseudocode (assuming we're allowed to alloc a new block):
					// If room in m_tail block, add to m_tail
					// Else check m_next block
					//     If m_next block is not the head block, push on m_next block
					//     Else create a new block and push there
					//     Advance m_tail to the block we just enqueued to

					memory_block *tail_block = m_tail_block.load();
					size_t block_front = tail_block->m_local_front;
					size_t block_tail = tail_block->m_tail.load();

					size_t next_block_tail = (block_tail + 1) & tail_block->m_size_mask;
					if (next_block_tail != block_front ||
						next_block_tail != (tail_block->m_local_front = tail_block->m_front.load()))
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						// This block has room for at least one more element
						char *location = tail_block->m_data + block_tail * sizeof(T);
						new(location) T(std::forward<U>(element));

						std::atomic_thread_fence(std::memory_order_release);
						tail_block->m_tail = next_block_tail;
					}
					else
					{
						std::atomic_thread_fence(std::memory_order_acquire);
						if (tail_block->m_next.load() != m_front_block)
						{
							// Note that the reason we can't advance to the m_front_block and start adding new entries there
							// is because if we did, then dequeue would stay in that block, eventually reading the new values,
							// instead of advancing to the m_next full block (whose values were enqueued first and so should be
							// consumed first).

							std::atomic_thread_fence(std::memory_order_acquire);        // Ensure we get latest writes if we got the latest m_front_block

							// m_tail_block is full, but there's a free block ahead, use it
							memory_block *tail_block_next = tail_block->m_next.load();
							size_t next_block_front = tail_block_next->m_local_front = tail_block_next->m_front.load();
							next_block_tail = tail_block_next->m_tail.load();
							std::atomic_thread_fence(std::memory_order_acquire);

							// This block must be empty since it's not the head block and we
							// go through the blocks in a circle
							assert(next_block_front == next_block_tail);
							tail_block_next->m_local_front = next_block_front;

							char *location = tail_block_next->m_data + next_block_tail * sizeof(T);
							new(location) T(std::forward<U>(element));

							tail_block_next->m_tail = (next_block_tail + 1) & tail_block_next->m_size_mask;

							std::atomic_thread_fence(std::memory_order_release);
							m_tail_block = tail_block_next;
						}
						else if (canAlloc == CAN_ALLOC) {
							// m_tail_block is full and there's no free block ahead; create a new block
							auto new_block_size =
									m_largest_block_size >= MAX_BLOCK_SIZE ? m_largest_block_size : m_largest_block_size * 2;
							auto new_block = make_block(new_block_size);
							if (new_block == nullptr) {
								// Could not allocate a block!
								return false;
							}
							m_largest_block_size = new_block_size;

							new(new_block->m_data) T(std::forward<U>(element));

							assert(new_block->m_front == 0);
							new_block->m_tail = new_block->m_local_tail = 1;

							new_block->m_next = tail_block->m_next.load();
							tail_block->m_next = new_block;

							// Might be possible for the dequeue thread to see the new m_tail_block->m_next
							// *without* seeing the new m_tail_block value, but this is OK since it can't
							// advance to the m_next block until m_tail_block is set anyway (because the only
							// case where it could try to read the m_next is if it's already at the m_tail_block,
							// and it won't advance past m_tail_block in any circumstance).

							std::atomic_thread_fence(std::memory_order_release);
							m_tail_block = new_block;
						}
						else if (canAlloc == CANNOT_ALLOC) {
							// Would have had to allocate a new block to push, but not allowed
							return false;
						}
						else {
							assert(false && "Should be unreachable code");
							return false;
						}
					}

					return true;
				}

			private:

				struct memory_block
				{
					// Avoid false-sharing by putting highly contended variables on their own cache lines
				public:
					std::atomic <size_t> 		m_front;    		// Elements are read from here
					size_t 						m_local_tail;       // An uncontended shadow copy of m_tail, owned by the consumer

					char cacheline_filler0[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>) - sizeof(size_t)];
					std::atomic <size_t> 		m_tail;    			// Elements are enqueued here
					size_t 						m_local_front;

					char cacheline_filler1[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>) - sizeof(size_t)];
					// m_next isn't very contended, but we don't want it on the same cache line as m_tail (which is)
					std::atomic<memory_block *> m_next;

					char*						m_data;		        // Contents (on heap) are aligned to T's alignment
					const size_t 				m_size_mask;

					// size must be a power of two (and greater than 0)
					memory_block(size_t const &size, char* raw_this, char* data)
							: m_front(0), m_local_tail(0), m_tail(0), m_local_front(0), m_next(nullptr), m_data(data),
							  m_size_mask(size - 1), m_raw_this(raw_this)
					{
					}

				private:
					memory_block &operator=(memory_block const &);

				public:
					char *m_raw_this;
				};


				static memory_block *make_block(size_t capacity)
				{
					// Allocate enough memory for the block itself, as well as all the elements it will contain
					auto size = sizeof(memory_block) + std::alignment_of<memory_block>::value - 1;
					size += sizeof(T) * capacity + std::alignment_of<T>::value - 1;
					auto new_block_raw = static_cast<char *>(std::malloc(size));
					if (new_block_raw == nullptr) 
					{
						return nullptr;
					}

					auto new_block_aligned = memory::align_for<memory_block>(new_block_raw);
					auto new_block_data = memory::align_for<T>(new_block_aligned + sizeof(memory_block));
					return new(new_block_aligned) memory_block(capacity, new_block_raw, new_block_data);
				}

			private:
				std::atomic<memory_block*>	m_front_block;		// Elements are enqueued to this block

				char cacheline_filler[CACHE_LINE_SIZE - sizeof(std::atomic<memory_block*>)];
				std::atomic<memory_block *> m_tail_block;      	// (Atomic) Elements are dequeued from this block

				size_t 						m_largest_block_size;
			};

			// Like lock_free_2points_queue, but also providees blocking operations
			template<typename T, size_t MAX_BLOCK_SIZE = 512>
			class blocking_2points_queue
			{
			private:
				typedef lock_free_2points_queue <T, MAX_BLOCK_SIZE> lock_free_2points_queue;

			public:
				explicit blocking_2points_queue(size_t maxSize = 15) : m_inner(maxSize) { }

				// Enqueues a copy of element if there is room in the queue.
				// Returns true if the element was enqueued, false otherwise.
				// Does not allocate memory.
				inline bool try_push(T const &element) 
				{
					if (m_inner.try_push(element))
					{
						m_semaphore.notify();
						return true;
					}
					return false;
				}

				// Enqueues a moved copy of element if there is room in the queue.
				// Returns true if the element was enqueued, false otherwise.
				// Does not allocate memory.
				inline bool try_push(T &&element)
				{
					if (m_inner.try_push(std::forward<T>(element)))
					{
						m_semaphore.notify();
						return true;
					}
					return false;
				}


				// Enqueues a copy of element on the queue.
				// Allocates an additional block of memory if needed.
				// Only fails (returns false) if memory allocation fails.
				inline bool push(T const &element) 
				{
					if (m_inner.push(element))
					{
						m_semaphore.notify();
						return true;
					}
					return false;
				}

				// Enqueues a moved copy of element on the queue.
				// Allocates an additional block of memory if needed.
				// Only fails (returns false) if memory allocation fails.
				inline bool push(T &&element) 
				{
					if (m_inner.push(std::forward<T>(element)))
					{
						m_semaphore.notify();
						return true;
					}
					return false;
				}


				// Attempts to dequeue an element; if the queue is empty,
				// returns false instead. If the queue has at least one element,
				// moves m_front to result using operator=, then returns true.
				template<typename U>
				bool try_pop(U &result) 
				{
					if (m_semaphore.try_wait())
					{
						bool success = m_inner.try_pop(result);
						assert(success);
						UNUSED(success);
						return true;
					}
					return false;
				}


				// Attempts to dequeue an element; if the queue is empty,
				// waits until an element is available, then dequeues it.
				template<typename U>
				void wait_pop(U &result) 
				{
					m_semaphore.wait();
					bool success = m_inner.try_pop(result);
					assert(success);
					UNUSED(success);
				}


				// Attempts to dequeue an element; if the queue is empty,
				// waits until an element is available up to the specified timeout,
				// then dequeues it and returns true, or returns false if the timeout
				// expires before an element can be dequeued.
				// Using a negative timeout indicates an indefinite timeout,
				// and is thus functionally equivalent to calling wait_pop.
				template<typename U>
				template<class Rep, class Period>
				bool wait_pop(U &result, const std::chrono::duration<Rep, Period> &d)
				{
					if (!m_semaphore.wait_for(d))
					{
						return false;
					}
					bool success = m_inner.try_pop(result);
					assert(success);
					UNUSED(success);
					return true;
				}


				// Returns a pointer to the m_front element in the queue (the one that
				// would be removed m_next by a call to `try_pop` or `pop`). If the
				// queue appears empty at the time the method is called, nullptr is
				// returned instead.
				// Must be called only from the consumer thread.
				inline T* peek() 
				{
					return m_inner.peek();
				}

				// Removes the m_front element from the queue, if any, without returning it.
				// Returns true on success, or false if the queue appeared empty at the time
				// `pop` was called.
				inline bool pop() 
				{
					if (m_semaphore.try_wait())
					{
						bool result = m_inner.pop();
						assert(result);
						UNUSED(result);
						return true;
					}
					return false;
				}

			private:
				blocking_2points_queue(lock_free_2points_queue const &) { }
				blocking_2points_queue &operator=(lock_free_2points_queue const &) { return *this; }

			private:
				lock_free_2points_queue 	m_inner;
				threading::weak_semaphore 	m_semaphore;
			};
		}
	}
}

#endif //GLADIUS_LOCK_FREE_SINGLE_QUEUE_H
