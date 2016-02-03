//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_RESOURCE_POOL_H
#define GLADIUS_RESOURCE_POOL_H

#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "../types.h"

#include "../memory/allocators/free_list_allocator.h"
#include "../memory/regions/stack_region.h"

namespace gladius
{
    namespace core
    {
        template<typename R, size_t SC, size_t DC>
        class c_resource_pool
        {
        public:
            R* get(handle_t handle)
            {
                return static_cast<R*>(m_index[handle]);
            }

            handle_t create()
            {
                handle_t handle = get_free_handle();
                if (handle == INVALID_HANDLE)
                {
                    return INVALID_HANDLE;
                }

                void* ptr = m_pool.alloc (sizeof(R));
                if (ptr == nullptr)
                {
                    return INVALID_HANDLE;
                }

                m_index[handle] = ptr;
                return handle;
            }

            handle_t create(handle_t handle)
            {
                void* ptr = m_index[handle];
                if (ptr != nullptr)
                {
                    return handle;
                }

                ptr = m_pool.alloc (sizeof(R));
                if (ptr == nullptr)
                {
                    return INVALID_HANDLE;
                }

                m_index[handle] = ptr;
                return handle;
            }

            void release(handle_t handle)
            {
                assert(INVALID_HANDLE != handle);

                void* ptr = m_index[handle];
                if (ptr == nullptr)
                {
                    return;
                }

                memset(ptr, 0, sizeof(R));
                m_pool.free (ptr);
                m_index[handle] = nullptr;
            }

        private:
            handle_t get_free_handle()
            {
                for (size_t i = SC; i < DC; i++)
                {
                    if (m_index[i] == nullptr)
                    {
                        return i;
                    }
                }
                return INVALID_HANDLE;
            }

        private:
            void* m_index[SC + DC];
            memory::c_free_list_allocator<sizeof(R), SC + DC, memory::c_stack_region<sizeof(R) * (SC + DC)>> m_pool;
        };
    }
}

#endif //GLADIUS_RESOURCE_POOL_H
