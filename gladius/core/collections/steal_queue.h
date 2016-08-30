//
// Created by pav on 11.09.15.
//

#ifndef GLADIUS_STEAL_QUEUE_H
#define GLADIUS_STEAL_QUEUE_H

#include <stddef.h>
#include <atomic>

namespace gladius {
namespace core {
namespace threading {

template<typename T, size_t CAPACITY>
class c_steal_queue {
private:
    static const size_t MASK = (CAPACITY - 1);

public:
    c_steal_queue() {
        m_bottom = 0;
        m_top = 0;
    }

public:
    bool push(T value) {
        size_t b = m_bottom;
        m_data[b & MASK] = value;

        std::atomic_signal_fence(std::memory_order_consume); // compiler barrier

        m_bottom = b + 1;
        return true;
    }

    bool pop(T& value) {
        size_t b = m_bottom - 1;
        m_bottom = b;

        size_t t = m_top.load(std::memory_order_seq_cst);

        if (t <= b) {
            // non-empty handle
            value = m_data[b & MASK];
            if (t != b) {
                // there's still more than one item left in the handle
                return true;
            }

            // this is the last item in the handle
            if (!m_top.compare_exchange_strong(t, t + 1)) // TO DO ensure memory ordering
            {
                // failed race against steal operation
                return false;
            }

            m_bottom = t + 1;
            return value;
        } else {
            // deque was already empty
            m_bottom = t;
            return false;
        }
    }

    bool steal(T& value) {
        size_t t = m_top.load(std::memory_order_consume);
        size_t b = m_bottom;
        if (t < b) {
            // non-empty handle
            value = m_data[t & MASK];

            // the interlocked function serves as a compiler barrier, and guarantees that the read happens before the CAS.
            if (!m_top.compare_exchange_strong(t, t + 1)) // TO DO ensure memory ordering
            {
                // a concurrent steal or pop operation removed an element from the deque in the meantime.
                return false;
            }
            return true;
        } else {
            // empty handle
            return false;
        }
    }

private:
    T m_data[CAPACITY];
    size_t m_bottom;
    std::atomic_size_t m_top;
};

}
}
}

#endif //GLADIUS_STEAL_QUEUE_H
