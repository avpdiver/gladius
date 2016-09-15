//
// Created by pav on 21.03.16.
//

#ifndef GLADIUS_LOCKFREE_POOL_H
#define GLADIUS_LOCKFREE_POOL_H

#include <cassert>
#include <atomic>
#include <memory>

#include "../memory_utils.h"

namespace gladius {
namespace core {
namespace memory {

template<typename TYPE>
class c_lockfree_pool {
    static constexpr size_t MAX_ALIGNMENT = alignof(TYPE);
    static constexpr size_t MAX_BLOCK_SIZE = sizeof(TYPE);

public:
    c_lockfree_pool(size_t size, void *start) {
        assert(start != nullptr);
		assert(is_aligned(start, MAX_ALIGNMENT));

        union {
            void *as_void;
            char *as_char;
            node_t *as_node;
        };

        as_void = start;

        tagget_ptr_t tagget_ptr = {as_node, 0};
        m_head.store(tagget_ptr);

        node_t *runner = as_node;
        as_char += MAX_BLOCK_SIZE;

        while (as_char != nullptr) {
            runner->next = as_node;
            runner = as_node;
            as_char += MAX_BLOCK_SIZE;
        }

        runner->next = nullptr;
    }

    inline void *alloc(size_t size = MAX_BLOCK_SIZE, size_t align = MAX_ALIGNMENT, size_t offset = 0) {
        assert (size <= MAX_BLOCK_SIZE && "Alloc size is greater then MAX_BLOCK_SIZE");
        assert (align <= MAX_ALIGNMENT && "Alignment is greater then MAX_ALIGNMENT");

        tagget_ptr_t current_head = m_head.load(std::memory_order_acquire);
        tagget_ptr_t new_head;
        while (current_head.ptr != nullptr) {
            new_head.ptr = current_head.ptr->next;
            new_head.tag = current_head.tag + 1;
            if (m_head.compare_exchange_weak(current_head, new_head, std::memory_order_release,
                                             std::memory_order_acquire)) {
                break;
            }
        }
        return current_head.ptr;
    }

    inline void free(void *ptr) {
        node_t *node = static_cast<node_t *>(ptr);

        tagget_ptr_t current_head = m_head.load(std::memory_order_relaxed);
        tagget_ptr_t new_head = {node};

        node->next = nullptr;

        do {
            new_head.tag = current_head.tag + 1;
            new_head.ptr = current_head.ptr;
        } while (!m_head.compare_exchange_weak(current_head, new_head, std::memory_order_release,
                                               std::memory_order_relaxed));
    }

private:
    struct node_t {
        node_t *next;
    };

    struct tagget_ptr_t {
        node_t *ptr;
        std::uintptr_t tag;
    };

    std::atomic<tagget_ptr_t> m_head;
};

}
}
}

#endif //GLADIUS_LOCKFREE_POOL_H
