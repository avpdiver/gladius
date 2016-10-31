//
// Created by pav on 21.03.16.
//

#ifndef GLADIUS_LOCKFREE_POOL_H
#define GLADIUS_LOCKFREE_POOL_H

#include <cassert>
#include <atomic>
#include <memory>

#include "../memory_utils.h"
#include "../tagged_pointer/tagged_ptr_packed.h"

namespace gladius {
namespace core {
namespace memory {

template<size_t MAX_BLOCK_SIZE, size_t MAX_ALIGNMENT>
class c_lockfree_alloc {
private:
    struct node_t {
        node_t *next;
    };
    typedef lockfree::tagged_ptr<node_t> tagget_ptr_t;

private:
    std::atomic<tagget_ptr_t> m_head;

public:
    c_lockfree_alloc(void *start, size_t size) {
        assert(start != nullptr);
		assert(is_aligned(start, MAX_ALIGNMENT));

        union {
            void *as_void;
            char *as_char;
            node_t *as_node;
        };

        as_void = start;

        tagget_ptr_t tagget_ptr(as_node, 0);
        m_head.store(tagget_ptr);

        node_t *runner = as_node;
        as_char += MAX_BLOCK_SIZE;

        while (--size > 0) {
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
        while (current_head.get_ptr() != nullptr) {
            new_head.set(current_head.get_ptr()->next, current_head.get_next_tag());
            if (m_head.compare_exchange_weak(current_head, new_head, std::memory_order_release,
                                             std::memory_order_acquire)) {
                break;
            }
        }
        return current_head.get_ptr();
    }

    inline void free(void *ptr) {
        node_t *node = static_cast<node_t *>(ptr);

        tagget_ptr_t current_head = m_head.load(std::memory_order_relaxed);
        tagget_ptr_t new_head(node);
        do {
            node->next = current_head.get_ptr();
        } while (!m_head.compare_exchange_weak(current_head, new_head, std::memory_order_release,
                                               std::memory_order_relaxed));
    }

    constexpr size_t size(void* ptr) const {
        return MAX_BLOCK_SIZE;
    }
};

}
}
}

#endif //GLADIUS_LOCKFREE_POOL_H
