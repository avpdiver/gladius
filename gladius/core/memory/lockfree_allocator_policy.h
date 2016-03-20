//
// Created by alexa on 023 23 02 2016.
//

#ifndef GLADIUS_LOCKFREE_ALLOCATOR_POLICY_H
#define GLADIUS_LOCKFREE_ALLOCATOR_POLICY_H

#include <cstddef>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <memory>
#include <cmath>
#include <atomic>
#include "../logging/logging.h"

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template<size_t CHUNK_SIZE, size_t ALIGNMENT, size_t MAX_ALLOC_SIZE>
            class c_lockfree_allocator_policy
            {
            private:
                struct s_partition
                {
                private:
                    struct s_chunk
                    {
                        s_chunk* prev;
                        s_chunk* next;
                        size_t size;
                    };

                private:
                    s_chunk* find_best(size_t chunks_number)
                    {
                        s_chunk* chunk = head;
                        s_chunk* best = head;
                        size_t max_size = 0;
                        while (chunk != nullptr)
                        {
                            if (chunk->size >= chunks_number && chunk->size < best->size)
                            {
                                best = chunk;
                                max_size = std::max(chunk->size - chunks_number, max_size);
                            }
                            else
                            {
                                max_size = std::max(chunk->size, max_size);
                            }

                            chunk = chunk->next;
                        }
                        max_free_size.store(max_size, std::memory_order_relaxed);
                        return best;
                    }

                public:
                    void init(uint8_t* _begin, size_t chunks_number)
                    {
                        begin = _begin;

                        chunks.resize(chunks_number);
                        head = &chunks[0];
                        head->prev = nullptr;
                        head->next = nullptr;
                        head->size = chunks_number;

                        max_free_size.store(chunks_number);
                    }

                public:
                    size_t get_max_free_size() const
                    {
                        return max_free_size.load(std::memory_order_relaxed);
                    }

                public:
                    void* alloc(size_t chunks_number)
                    {
                        s_chunk* chunk = find_best(chunks_number);
                        if (chunk == nullptr)
                        {
                            return nullptr;
                        }

                        s_chunk* next;
                        if (chunk->size > chunks_number)
                        {
                            size_t diff = chunk->size - chunks_number;
                            chunk->size = chunks_number;
                            next = chunk + chunks_number;
                            next->next = chunk->next;
                        }
                        else
                        {
                            next = chunk->next;
                        }

                        if (chunk->prev != nullptr)
                        {
                            chunk->prev->next = next;
                        }
                        else
                        {
                            head = next;
                            head->prev = nullptr;
                        }

                        return static_cast<void*>(begin + (chunk - &chunks[0]) * CHUNK_SIZE);
                    }

                    void free(void* ptr)
                    {
                        size_t index = (static_cast<uint8_t*>(ptr) - begin) / CHUNK_SIZE;
                        auto chunk = &chunks[index];
                        if (head == nullptr)
                        {
                            head = chunk;
                            head->next = nullptr;
                            head->prev = nullptr;
                            return;
                        }

                        if (chunk < head)
                        {
                            head->prev = chunk;
                            chunk->next = head;
                            head = chunk;
                        }
                        else
                        {
                            auto c = head;
                            while (c != nullptr)
                            {
                                if (c->next > chunk)
                                {
                                    chunk->next = c->next;
                                    chunk->prev = c;
                                    c->next = chunk;
                                    c->next->prev = chunk;
                                    break;
                                }
                                else if (c->next == nullptr)
                                {
                                    chunk->next = nullptr;
                                    chunk->prev = c;
                                    c->next = chunk;
                                    break;
                                }
                                c = c->next;
                            }
                        }

                        size_t merged_size = chunk->size;

                        // merge free chunks
                        if (chunk + chunk->size == chunk->next)
                        {
                            chunk->size += chunk->next->size;
                            chunk->next = chunk->next->next;
                            merged_size = chunk->size;
                        }

                        if (chunk->prev != nullptr && chunk->prev == (chunk - chunk->prev->size))
                        {
                            chunk->prev->size += chunk->size;
                            chunk->prev->next = chunk->next;
                            chunk->next->prev = chunk->prev;
                            merged_size = chunk->prev->size;
                        }

                        max_free_size.store(std::max(merged_size, max_free_size.load(std::memory_order_relaxed)));
                    }

                private:
                    uint8_t* begin = nullptr;

                private:
                    s_chunk* head = nullptr;
                    std::vector<s_chunk> chunks;

                private:
                    std::atomic_size_t max_free_size;
                };

            public:
                c_lockfree_allocator_policy(uint8_t *begin, uint8_t *end)
                {
                    size_t size = end - begin;
                    begin = static_cast<uint8_t*>(std::align(ALIGNMENT, CHUNK_SIZE, static_cast<void *>(begin), size));
                    size = end - begin;
                    assert(size >= CHUNK_SIZE);
                    assert(size >= MAX_ALLOC_SIZE);

                    size_t chunks_number = size / CHUNK_SIZE;
                    size_t chunks_per_max_alloc = (size_t) std::ceil((double)MAX_ALLOC_SIZE / (double)CHUNK_SIZE);
                    chunks_per_max_alloc = 1UL <<(1 +(63 -__builtin_clzl(chunks_per_max_alloc - 1))); // round up to nearest power of 2
                    size_t partition_size = chunks_per_max_alloc * CHUNK_SIZE;
                    assert(size >= partition_size);

                    size_t partitions_number = size / partitions_number;

                    m_partions.resize(partitions_number);
                    uint8_t *runner = begin;
                    for (auto &p : m_partions)
                    {
                        p.init(runner, chunks_per_max_alloc);
                        runner += partition_size;
                        m_thread_partitions.push_back(&p);
                    }
                }

            public:
                void* alloc(size_t size)
                {

                }

                void free(void* ptr)
                {

                }
                
            private:
                std::vector<s_partition> m_partions;
                thread_local std::vector<s_partition*> m_thread_partitions;
            };
        }
    }
}

#endif //GLADIUS_LOCKFREE_ALLOCATOR_POLICY_H
