//
// Created by avpdiver on 31.08.16.
//

#ifndef GLADIUS_BLOCKING_QUEUE_H
#define GLADIUS_BLOCKING_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace gladius {
namespace core {
namespace collections {

template<typename T>
class c_blocking_queue {
public:
	void push(T const &value) {
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.push_front(value);
		}
		m_condition.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_queue.empty()) {
			m_condition.wait(lock, [=] { return !m_queue.empty(); });
		}
		T rc(std::move(m_queue.back()));
		m_queue.pop_back();
		return rc;
	}

	bool try_pop(T& value) {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_queue.empty()) {
			return false;
		}
		value = std::move(m_queue.back());
		m_queue.pop_back();
		return true;
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::deque<T> m_queue;
};

}
}
}

#endif //GLADIUS_BLOCKING_QUEUE_H
