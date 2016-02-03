//
// Created by pav on 11.08.15.
//

#ifndef GLADIUS_STACK_REGION_H
#define GLADIUS_STACK_REGION_H

#include <stddef.h>
#include <stdint.h>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template <size_t SIZE>
            class c_stack_region
            {
            public:
                c_stack_region() : pointer(buf)
                {

                }

            public:
                void* alloc(size_t size)
                {
                    if (size > SIZE || pointer > buf + SIZE - size)
                    {
                        return nullptr;
                    }
                    void* r = pointer;
                    pointer += size;
                    return r;
                }

                void free_all()
                {
                    pointer = buf;
                }

            private:
                uint8_t buf[SIZE], *pointer;
            };
        }
    }
}


#endif //GLADIUS_STACK_REGION_H
