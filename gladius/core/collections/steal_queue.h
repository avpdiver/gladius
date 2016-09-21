//
// Created by pav on 11.09.15.
//

#ifndef GLADIUS_STEAL_QUEUE_H
#define GLADIUS_STEAL_QUEUE_H

#include <stddef.h>
#include <atomic>
#include <vector>

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t CAPACITY>
class c_steal_queue {
private:
    static const size_t MASK = (CAPACITY - 1);

public:
    c_steal_queue() : m_bottom(0), m_top(0) {
    }

public:
    DEFAULT_MOVE_ONLY(c_steal_queue);

public:
    bool push(T const& value) {
        ssize_t b = m_bottom;
        m_data[b & MASK] = value;

        std::atomic_signal_fence(std::memory_order_seq_cst); // compiler barrier

        m_bottom = b + 1;
        return true;
    }

    bool push(T&& value) {
        ssize_t b = m_bottom;
        m_data[b & MASK] = std::move(value);

        std::atomic_signal_fence(std::memory_order_seq_cst); // compiler barrier

        m_bottom = b + 1;
        return true;
    }

    bool pop(T& value) {
        ssize_t b = m_bottom - 1;
        m_bottom = b;

		std::atomic_thread_fence(std::memory_order_seq_cst); // compiler barrier

        ssize_t t = m_top.load(std::memory_order_seq_cst);

        if (t <= b) {
            value = std::move(m_data[b & MASK]);
            if (t != b) {
                return true;
            }
			bool res = true;
            if (!m_top.compare_exchange_strong(t, t + 1)) {
                res = false;
            }
            m_bottom = t + 1;
            return res;
        } else {
            // deque was already empty
            m_bottom = t;
            return false;
        }
    }

    bool steal(T& value) {
        ssize_t t = m_top.load(std::memory_order_seq_cst);
        std::atomic_signal_fence(std::memory_order_seq_cst); // compiler barrier
        ssize_t b = m_bottom;

        if (t < b) {
            value = std::move(m_data[t & MASK]);
            if (!m_top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst)) {
                return false;
            }
            return true;
        } else {
            return false;
        }
    }

private:
    std::array<T, CAPACITY> m_data;
    ssize_t m_bottom;
    std::atomic<ssize_t> m_top;
};

}
}
}

#endif //GLADIUS_STEAL_QUEUE_H
