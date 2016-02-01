//
// Created by pav on 11.08.15.
//

#ifndef DAR_HEAP_REGION_H
#define DAR_HEAP_REGION_H

#include <stddef.h>
#include <stdint.h>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template<size_t SIZE>
            class c_heap_region
            {
            public:
                c_heap_region () : buf (new uint8_t[SIZE]), pointer (buf)
                {

                }

                ~c_heap_region ()
                {
                    delete[] buf;
                    pointer = nullptr;
                }

            public:
                void *alloc (size_t size)
                {
                    if (size > SIZE || pointer > buf + SIZE - size)
                    {
                        return nullptr;
                    }
                    void *r = pointer;
                    pointer += size;
                    return r;
                }

                void free_all ()
                {
                    pointer = buf;
                }

            private:
                uint8_t *buf, *pointer;
            };
        }
    }
}


#endif //DAR_HEAP_REGION_H
