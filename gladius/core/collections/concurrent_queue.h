//
// Created by avpdiver on 30.08.16.
//

#ifndef GLADIUS_CONCURRENT_QUEUE_H
#define GLADIUS_CONCURRENT_QUEUE_H

#include <cstddef>
#include <atomic>
#include <condition_variable>
#include "../types.h"
#include "../memory/tagged_pointer/tagged_ptr_packed.h"
#include "../memory/alloc_policies/lockfree_alloc.h"

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t CAPACITY>
class c_concurrent_queue {
private:
    struct node_t;
    typedef lockfree::tagged_ptr<node_t> ptr_t;

    struct node_t {
    public:
        node_t() : next(ptr_t(nullptr)), dummy(true) {
        }

        node_t(T const &v) : next(ptr_t(nullptr)), dummy(false) {
            new(value) T(v);
        }

        node_t(T &&v) : next(ptr_t(nullptr)), dummy(false) {
            new(value) T(v);
        }

    public:
        void copy(T &v) {
            new(&v) T(std::move(*reinterpret_cast<T *>(value)));
        }

    public:
        std::atomic<ptr_t> next;

    protected:
        uint8_t value[sizeof(T)];
        bool dummy;
    };

private:
    struct pool_t {
    public:
        pool_t() : m_allocator(m_storage) {}

    public:
        node_t *alloc(T &&value) {
            node_t *ptr = (node_t *) m_allocator.alloc();
            if (ptr == nullptr) {
                return nullptr;
            }
            new(ptr) node_t(value);
            return ptr;
        }

        void free(node_t *node) {
            m_allocator.free(node);
        }

    private:
        typedef typename std::aligned_storage<sizeof(node_t), alignof(node_t)>::type storage_t;
        storage_t m_storage[CAPACITY];
        memory::c_lockfree_alloc<CAPACITY, sizeof(storage_t), alignof(storage_t)> m_allocator;
    };

private:
    pool_t m_pool;
    std::atomic<ptr_t> m_head;
    std::atomic<ptr_t> m_tail;
    std::atomic<size_t> m_size;
    std::condition_variable m_notifier;
    std::mutex m_mutex;

public:
    c_concurrent_queue() : m_size(0) {
        static node_t dummy;
        m_head.store(ptr_t(&dummy));
        m_tail.store(ptr_t(&dummy));
    }

    NOT_COPYABLE(c_concurrent_queue);
    NOT_MOVEABLE(c_concurrent_queue);

public:
    bool push(T &&value) {
        node_t *node = m_pool.alloc(std::move(value));
        if (node == nullptr) {
            return false;
        }

        for (;;) {
            ptr_t tail = m_tail.load(std::memory_order_acquire);
            node_t *tail_node = tail.get_ptr();

            ptr_t next = tail_node->next.load(std::memory_order_acquire);
            node_t *next_node = next.get_ptr();

            ptr_t tail2 = m_tail.load(std::memory_order_acquire);
            if (tail == tail2) {
                if (next_node == nullptr) {
                    ptr_t new_tail_next(node, next.get_next_tag());
                    if (tail_node->next.compare_exchange_weak(next, new_tail_next)) {
                        ptr_t new_tail(node, tail.get_next_tag());
                        m_tail.compare_exchange_strong(tail, new_tail);
                        m_size++;
                        m_notifier.notify_one();
                        return true;
                    }
                } else {
                    ptr_t new_tail(next_node, tail.get_next_tag());
                    m_tail.compare_exchange_strong(tail, new_tail);
                }
            }
        }
    }

    bool try_pop(T &value) {
        for (;;) {
            ptr_t head = m_head.load(std::memory_order_acquire);
            node_t *head_node = head.get_ptr();

            ptr_t tail = m_tail.load(std::memory_order_acquire);
            ptr_t next = head_node->next.load(std::memory_order_acquire);
            node_t *next_node = next.get_ptr();

            ptr_t head2 = m_head.load(std::memory_order_acquire);
            if (head == head2) {
                if (head_node == tail.get_ptr()) {
                    if (next_node == nullptr) {
                        return false;
                    }
                    ptr_t new_tail(next_node, tail.get_next_tag());
                    m_tail.compare_exchange_strong(tail, new_tail);
                } else {
                    if (next_node == nullptr) {
                        continue;
                    }
                    next_node->copy(value);
                    ptr_t new_head(next_node, head.get_next_tag());
                    if (m_head.compare_exchange_weak(head, new_head)) {
                        m_pool.free(head.get_ptr());
                        m_size--;
                        return true;
                    }
                }
            }
        }
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (true) {
            if (m_size.load() == 0) {
                m_notifier.wait(lock);
            }
            if (try_pop(value)) {
                return true;
            }
        }
        return false;
    }
};

}
}
}
#endif //GLADIUS_CONCURRENT_QUEUE_H
