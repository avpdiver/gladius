//
// Created by avpdiver on 31.08.16.
//

#ifndef GLADIUS_BLOCKING_QUEUE_H
#define GLADIUS_BLOCKING_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

#include "../types.h"
#include "steal_queue.h"

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t CAPACITY>
class c_blocking_queue {
private:
    using queue_t = c_steal_queue<T, CAPACITY>;

public:
    c_blocking_queue() {}

    MOVE_ONLY(c_blocking_queue);

public:
    ~c_blocking_queue() {
        delete m_queue.load(std::memory_order_acquire);
    }

public:
	void push(T const& value) {
        auto queue = get_current_queue();
        queue->push(value);
		m_condition.notify_one();
	}

    void push(T&& value) {
        auto queue = get_current_queue();
        queue->push(std::move(value));
        m_condition.notify_one();
    }

    bool try_pop(T& value) {
        auto queue = get_current_queue();
        if (queue->pop(value)) {
            return true;
        }

        auto ctx = m_queue.load(std::memory_order_acquire);
        for (auto q = ctx; q != nullptr; q = q->next) {
            if (&q->queue == queue) {
                continue;
            }
            if (q->queue.steal(value)) {
                return true;
            }
        }
        return false;
    }

	void pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto queue = get_current_queue();
        bool wait = true;
        while (wait) {
            m_condition.wait(lock, [&] {
                if (queue->pop(value)) {
                    wait = false;
                    return true;
                }
                auto ctx = m_queue.load(std::memory_order_acquire);
                for (auto q = ctx; q != nullptr; q = q->next) {
                    if (&q->queue == queue) {
                        continue;
                    }
                    if (q->queue.steal(value)) {
                        wait = false;
                        return true;
                    }
                }
                return false;
            });
        }
    }

private:
	typedef std::uintptr_t thread_id_t;

	thread_id_t get_thread_id() {
		static thread_local size_t id;
		return reinterpret_cast<thread_id_t>(&id);
	}

    queue_t* get_current_queue() {
        auto thread_id = get_thread_id();

        auto queue = m_queue.load(std::memory_order_acquire);
        for (auto q = queue; q != nullptr; q = q->next) {
            if (q->thread_id == thread_id) {
                return &(q->queue);
            }
        }

        auto ctx = new s_thread_context(thread_id, queue);
        m_queue.store(ctx);
        return &(ctx->queue);
    }

private:
    struct s_thread_context {
        thread_id_t thread_id;
        s_thread_context* next;
        c_steal_queue<T, CAPACITY> queue;

        s_thread_context(thread_id_t id, s_thread_context* n) : thread_id(id), next(n) {}
        ~s_thread_context() {
            delete next;
        }
    };

    std::atomic<s_thread_context*> m_queue{nullptr};

private:
	std::mutex m_mutex;
	std::condition_variable m_condition;

};

}
}
}

#endif //GLADIUS_BLOCKING_QUEUE_H
