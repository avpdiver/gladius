//
// Created by pav on 11.08.15.
//

#ifndef GLADIUS_SIZED_ALLOCATOR_H
#define GLADIUS_SIZED_ALLOCATOR_H

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template<typename BASE_ALLOCATOR>
            class c_sized_allocator : private BASE_ALLOCATOR
            {

            public:
                void *alloc (size_t size)
                {
                    size_t *s = static_cast<size_t *>(BASE_ALLOCATOR::allocate (size + sizeof (size_t)));
                    return *s = size, s + 1;
                }

                void free (void *ptr)
                {
                    BASE_ALLOCATOR::free (static_cast<size_t *>(ptr) - 1);
                }

                size_t size (void *ptr)
                {
                    return (static_cast<size_t *>(ptr))[-1];
                }
            };
        }
    }
}

#endif //GLADIUS_SIZED_ALLOCATOR_H
