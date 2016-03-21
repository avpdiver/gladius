//
// Created by pav on 21.03.16.
//

#ifndef GLADIUS_DEFAULT_POLICIES_H
#define GLADIUS_DEFAULT_POLICIES_H

#include <cstddef>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
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

#endif //GLADIUS_DEFAULT_POLICIES_H
