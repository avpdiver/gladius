//
// Created by pav on 21.03.16.
//

#ifndef GLADIUS_LINEAR_ALLOC_POLICY_H
#define GLADIUS_LINEAR_ALLOC_POLICY_H

#include "../alloc_policy.h"

namespace gladius {
namespace core {
namespace memory {

class c_linear_alloc : public c_alloc_policy {
public:
    c_linear_alloc(size_t size, void *start)
        : c_alloc_policy(size, start), m_current_pos(start) {
    }

    ~c_linear_alloc() {
        m_current_pos = nullptr;
    }

public:
    void *alloc(size_t size, size_t alignment, size_t offset) {
        void *ptr = std::align(alignment, size, m_current_pos + offset, (m_size - m_used_memory)) - offset;
        if (ptr == nullptr) {
            return nullptr;
        }

        m_used_memory += static_cast<uint8_t *>(ptr) + size - static_cast<uint8_t *>(m_current_pos);
        m_current_pos = static_cast<uint8_t *>(prt) + size;
        m_num_allocations++;

        return (void *) ptr;
    }

    void free(void *p) override {

    }

public:
    void clear() {
        m_num_allocations = 0;
        m_used_memory = 0;
        m_current_pos = m_start;
    }

private:
    c_linear_alloc(const c_linear_alloc &); //Prevent copies because it might cause errors
    c_linear_alloc &operator=(const c_linear_alloc &);

private:
    void *m_current_pos;
};

}
}
}

#endif //GLADIUS_LINEAR_ALLOC_POLICY_H
