//
// Created by pav on 11.08.15.
//

#ifndef DAR_FREE_LIST_ALLOCATOR_H
#define DAR_FREE_LIST_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

namespace gladius
{
    namespace core
    {
        namespace memory
        {
            template<size_t ELEM_SIZE, size_t ELEM_COUNT, typename BASE_ALLOCATOR>
            class c_free_list_allocator : private BASE_ALLOCATOR
            {
            private:
                struct s_list { s_list* m_next; };

            public:
                c_free_list_allocator()
                {
                    m_base_ptr = BASE_ALLOCATOR::alloc(ELEM_COUNT * ELEM_SIZE);
                    m_next = static_cast<s_list*>(m_base_ptr);

                    union
                    {
                        uint8_t* uint8_ptr;
                        s_list* list_ptr;
                    };

                    list_ptr = m_next;
                    uint8_ptr += ELEM_SIZE;
                    s_list* list = m_next;
                    for (size_t i = 1; i < ELEM_COUNT; i++)
                    {
                        list->m_next = list_ptr;
                        list = list_ptr;
                        uint8_ptr += ELEM_SIZE;
                    }
                    list->m_next = nullptr;
                }

                ~c_free_list_allocator ()
                {
                    BASE_ALLOCATOR::free(m_base_ptr);
                }

            public:
                void* alloc(size_t size)
                {
                    if (size > ELEM_SIZE || m_next == nullptr)
                    {
                        return BASE_ALLOCATOR::alloc(size);
                    }

                    s_list* head = m_next;
                    m_next = head->m_next;
                    return head;
                }

                void free(void* ptr)
                {
                    s_list* head = static_cast<s_list*>(ptr);
                    head->m_next = m_next;
                    m_next = head;
                }

            private:
                s_list* m_next;
                void* m_base_ptr;
            };
        }
    }
}

#endif //DAR_FREE_LIST_ALLOCATOR_H
