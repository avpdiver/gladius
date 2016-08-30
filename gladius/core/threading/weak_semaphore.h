//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_WEAK_SEMAPHORE_H
#define GLADIUS_WEAK_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace gladius {
namespace core {
namespace threading {

template<typename Mutex, typename CondVar>
class c_basic_semaphore {
public:
    using native_handle_type = typename CondVar::native_handle_type;

    explicit c_basic_semaphore(size_t count = 0);
    c_basic_semaphore(const c_basic_semaphore &) = delete;
    c_basic_semaphore(c_basic_semaphore &&) = delete;
    c_basic_semaphore &operator=(const c_basic_semaphore &) = delete;
    c_basic_semaphore &operator=(c_basic_semaphore &&) = delete;

    void notify();
    void wait();
    bool try_wait();
    template<class Rep, class Period>
    bool wait_for(const std::chrono::duration<Rep, Period> &d);
    template<class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration> &t);

    native_handle_type native_handle();

private:
    Mutex m_mutex;
    CondVar m_cv;
    size_t m_count;
};

using semaphore = c_basic_semaphore<std::mutex, std::condition_variable>;

template<typename Mutex, typename CondVar>
c_basic_semaphore<Mutex, CondVar>::c_basic_semaphore(size_t count)
    : m_count {count} {}

template<typename Mutex, typename CondVar>
void c_basic_semaphore<Mutex, CondVar>::notify() {
    std::lock_guard<Mutex> lock {m_mutex};
    ++m_count;
    m_cv.notify_one();
}

template<typename Mutex, typename CondVar>
void c_basic_semaphore<Mutex, CondVar>::wait() {
    std::unique_lock<Mutex> lock {m_mutex};
    m_cv.wait(lock, [&] { return m_count > 0; });
    --m_count;
}

template<typename Mutex, typename CondVar>
bool c_basic_semaphore<Mutex, CondVar>::try_wait() {
    std::lock_guard<Mutex> lock {m_mutex};

    if (m_count > 0) {
        --m_count;
        return true;
    }

    return false;
}

template<typename Mutex, typename CondVar>
template<class Rep, class Period>
bool c_basic_semaphore<Mutex, CondVar>::wait_for(const std::chrono::duration<Rep, Period> &d) {
    std::unique_lock<Mutex> lock {m_mutex};
    auto finished = m_cv.wait_for(lock, d, [&] { return m_count > 0; });

    if (finished)
        --m_count;

    return finished;
}

template<typename Mutex, typename CondVar>
template<class Clock, class Duration>
bool c_basic_semaphore<Mutex, CondVar>::wait_until(const std::chrono::time_point<Clock, Duration> &t) {
    std::unique_lock<Mutex> lock {m_mutex};
    auto finished = m_cv.wait_until(lock, t, [&] { return m_count > 0; });

    if (finished)
        --m_count;

    return finished;
}

template<typename Mutex, typename CondVar>
typename c_basic_semaphore<Mutex, CondVar>::native_handle_type c_basic_semaphore<Mutex, CondVar>::native_handle() {
    return m_cv.native_handle();
}

typedef c_basic_semaphore<std::mutex, std::condition_variable> c_weak_semaphore;

}
}
}

#endif //GLADIUS_WEAK_SEMAPHORE_H
