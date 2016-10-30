//
// Created by avpdiver on 30.08.16.
//

#ifndef GLADIUS_CONCURRENT_QUEUE_H
#define GLADIUS_CONCURRENT_QUEUE_H

#include <cstddef>
#include <atomic>
#include "../types.h"
#include "details/tagged_ptr_packed.h"

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t CAPACITY>
class c_concurrent_queue {
private:
    static const size_t MASK = (CAPACITY - 1);

private:
    T m_pool[CAPACITY];
    std::atomic<size_t> m_tail;
    std::atomic<size_t> m_head;

public:
    c_concurrent_queue() : m_head(0), m_tail(0) {}

    NOT_COPYABLE(c_concurrent_queue);
    NOT_MOVEABLE(c_concurrent_queue);

public:
    bool push(T const &value) {
        size_t new_tail;
        for (;;) {
            size_t tail = m_tail.load(std::memory_order_acquire);
            size_t head = m_head.load(std::memory_order_acquire);
            if (tail - head >= CAPACITY) {
                return false; // full
            }

            new_tail = tail + 1;
            if (m_tail.compare_exchange_weak(tail, new_tail)) {
                m_pool[tail & MASK] = value;
                return true;
            }
        }
    }

    bool push(T &&value) {
        size_t new_tail;
        for (;;) {
            size_t tail = m_tail.load(std::memory_order_acquire);
            size_t head = m_head.load(std::memory_order_acquire);
            if (tail - head >= CAPACITY) {
                return false;
            }

            new_tail = tail + 1;
            if (m_tail.compare_exchange_weak(tail, new_tail)) {
                m_pool[tail & MASK] = std::move(value);
                return true;
            }
        }
    }

    bool pop(T &value) {
        size_t new_head;
        for (;;) {
            size_t tail = m_tail.load(std::memory_order_acquire);
            size_t head = m_head.load(std::memory_order_acquire);
            if (tail == head) {
                return false; // empty
            }
            new_head = head + 1;
            if (m_head.compare_exchange_weak(head, new_head)) {
                value = m_pool[head & MASK];
                return true;
            }
        }
    }
};

}
}
}
#endif //GLADIUS_CONCURRENT_QUEUE_H
