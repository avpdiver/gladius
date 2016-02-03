//
// Created by pav on 11.08.15.
//

#include <stdlib.h>
#include <malloc.h>

#include "malloc_allocator.h"

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            void *c_malloc_allocator::alloc (size_t size)
            {
                return malloc(size);
            }

            void c_malloc_allocator::free (void *ptr)
            {
                ::free (ptr);
            }

            size_t c_malloc_allocator::size (void *ptr)
            {
#if defined(WINDOWS)
                    return _msize(ptr);
#elif defined(LINUX)
                    return malloc_usable_size(ptr);
#endif
            }
        }
    }
}
