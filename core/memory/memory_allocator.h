//
// Created by alexa on 023 23 02 2016.
//

#ifndef GLADIUS_MEMORY_ALLOCATOR_H
#define GLADIUS_MEMORY_ALLOCATOR_H

#include <cstddef>
#include <stdint.h>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template<class ALLOC_POLICY, class THREAD_POLICY, class BOUNDS_POLICY, class TRACKING_POLICY, class TAGGING_POLICY>
            class c_memory_allocator
            {
            public:
                template<class AREA_POLICY>
                explicit c_memory_allocator(const AREA_POLICY &area) : m_allocator(area.get_start(), area.get_end())
                {
                }

                c_memory_allocator(uint8_t* start, size_t size) : m_allocator(start, start + size)
                {
                }

                c_memory_allocator(uint8_t* start, uint8_t* end) : m_allocator(start, end)
                {
                }

            public:
                void *alloc(size_t size, size_t align)
                {
                    m_thread_guard.lock();

                    const size_t original_size = size;
                    const size_t new_size = size + BOUNDS_POLICY::SIZE_FRONT + BOUNDS_POLICY::SIZE_BACK;
                    char *ptr = static_cast<char*>(m_allocator.alloc(new_size, align, BOUNDS_POLICY::SIZE_FRONT));

                    m_bounds_checker.guard_front(ptr);
                    m_bounds_checker.guard_back(ptr + BOUNDS_POLICY::SIZE_FRONT + original_size);
                    m_tagger.alloc(ptr + BOUNDS_POLICY::SIZE_FRONT, original_size);
                    m_tracker.alloc(ptr, new_size, align);

                    m_thread_guard.unlock();

                    return (ptr + BOUNDS_POLICY::SIZE_FRONT);
                }

                void free(void *ptr)
                {
                    m_thread_guard.lock();

                    char *original = static_cast<char *>(ptr) - BOUNDS_POLICY::SIZE_FRONT;
                    const size_t size = m_allocator.get_size(original);

                    m_bounds_checker.check_front(original);
                    m_bounds_checker.check_back(original + size - BOUNDS_POLICY::SIZE_BACK);
                    m_tracker.free(original);
                    m_tagger.free(original, size);
                    m_allocator.free(original);

                    m_thread_guard.unlock();
                }

            private:
                ALLOC_POLICY m_allocator;
                THREAD_POLICY m_thread_guard;
                BOUNDS_POLICY m_bounds_checker;
                TRACKING_POLICY m_tracker;
                TAGGING_POLICY m_tagger;
            };

            class c_no_bounds_policy
            {
            public:
                static const size_t SIZE_FRONT = 0;
                static const size_t SIZE_BACK = 0;

                inline void guard_front(void*) const {}
                inline void guard_back(void*) const {}

                inline void check_front(const void*) const {}
                inline void check_back(const void*) const {}
            };

            class c_no_tracking_policy
            {
            public:
                inline void alloc(void*, size_t, size_t) const {}
                inline void free(void*) const {}
            };

            class c_no_tagging_policy
            {
            public:
                inline void alloc(void*, size_t) const {}
                inline void free(void*, size_t) const {}
            };

            class c_single_thread_policy
            {
            public:
                inline void lock(void) const {}
                inline void unlock(void) const {}
            };
        }
    }
}

#endif //GLADIUS_MEMORY_ALLOCATOR_H
