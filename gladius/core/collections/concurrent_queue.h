//
// Created by avpdiver on 30.08.16.
//

#ifndef GLADIUS_CONCURRENT_QUEUE_H
#define GLADIUS_CONCURRENT_QUEUE_H

#include <cstddef>
#include <mutex>
#include <condition_variable>

#include "../types.h"
#include "steal_queue.h"

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t CAPACITY>
class c_concurrent_queue {
private:
	using thread_id_t = std::uintptr_t;
    using queue_t = c_steal_queue<T, CAPACITY>;

	struct alignas(4 * sizeof(uintptr_t)) s_thread_context {
		thread_id_t thread_id;
		s_thread_context* next;
		c_steal_queue<T, CAPACITY>* queue;

		s_thread_context(thread_id_t id, s_thread_context* n)
			: thread_id(id), next(n), queue(new c_steal_queue<T, CAPACITY>()) {}
		~s_thread_context() {
			delete queue;
			delete next;
		}
	};

private:
	std::atomic_size_t m_thread_number{0};
	std::atomic_size_t m_pop_index{0};
	std::atomic<s_thread_context*> m_queue{nullptr};
	std::mutex m_mutex;
	std::condition_variable m_condition;

public:
    c_concurrent_queue() {}

    DEFAULT_MOVE_ONLY(c_concurrent_queue);

public:
    ~c_concurrent_queue() {
        delete m_queue.load(std::memory_order_acquire);
    }

public:
    void push(T const& value) {
        auto ctx = get_current_queue();
        ctx->queue->push(value);
        m_condition.notify_one();
    }

    void push(T&& value) {
        auto ctx = get_current_queue();
        ctx->queue->push(std::move(value));
        m_condition.notify_one();
    }

    bool try_pop(T& value) {
		size_t thread_number = m_thread_number.load(std::memory_order_acquire);
		if (thread_number == 0) {
			return false;
		}

		auto ctx = m_queue.load(std::memory_order_acquire);
		if (ctx == nullptr) {
			return false;
		}

		size_t pop_index = m_pop_index++;
		auto thread_id = get_thread_id();

		while (thread_number > 0) {
			pop_index %= thread_number;
			size_t i = 0;
			for (auto c = ctx; c != nullptr; c = c->next, i++) {
				if (pop_index != i) {
					continue;
				}
				if ((c->thread_id == thread_id && c->queue->pop(value)) || c->queue->steal(value)) {
					return true;
				} else {
					break;
				}
			}
			pop_index++;
			thread_number--;
		}
		return false;
	}

    void pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool wait = true;
        while (wait) {
            m_condition.wait(lock, [&] {
				wait = !try_pop(value);
				return !wait;
            });
        }
    }

private:
    thread_id_t get_thread_id() {
        static thread_local size_t id;
        return reinterpret_cast<thread_id_t>(&id);
    }

	s_thread_context* get_current_queue() {
        auto thread_id = get_thread_id();
        auto queue = m_queue.load(std::memory_order_acquire);
        for (auto q = queue; q != nullptr; q = q->next) {
            if (q->thread_id == thread_id) {
                return q;
            }
        }
        auto ctx = new s_thread_context(thread_id, queue);
        m_queue.store(ctx);
		m_thread_number++;
        return ctx;
    }
};

}
}
}
#endif //GLADIUS_CONCURRENT_QUEUE_H
