//
// Created by pav on 11.08.15.
//

#ifndef DAR_MALLOC_ALLOCATOR_H
#define DAR_MALLOC_ALLOCATOR_H

#include <stddef.h>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            class c_malloc_allocator
            {
            public:
                void* alloc(size_t size);
                void free(void* ptr);
                size_t size(void* ptr);
            };
        }
    }
}

#endif //DAR_MALLOC_ALLOCATOR_H
