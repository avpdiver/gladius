//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_WORKER_H
#define GLADIUS_WORKER_H

#include <thread>

#include "../meta/fixed_function.h"
#include "movable_atomic.h"

namespace gladius {
namespace core {
namespace threading {

using task_t = core::fixed_function<void(), 128>;
using task_queue_t = collections::c_blocking_queue<task_t, 8>;

class c_worker {
private:
    enum class e_state {
        uninitialized,
        running,
        stopped,
        finished
    };

private:
    std::thread m_thread;
    task_queue_t* m_queue;
    movable_atomic<e_state> m_state{e_state::uninitialized};

private:

    void run() {
        assert(m_state == e_state::running);

        task_t t;
        while (m_state == e_state::running) {
            m_queue->pop(t);
            t();
        }
        m_state = e_state::finished;
    }

public:
    c_worker(task_queue_t *queue) noexcept : m_queue{queue} {
    }

public:
    MOVE_ONLY(c_worker);

public:
    template<typename COUNTER>
    void start(COUNTER &remaining_inits) {
        assert(m_state == e_state::uninitialized);

        m_thread = std::thread([this, &remaining_inits] {
            m_state = e_state::running;
            (--remaining_inits);
            run();
        });
    }

    void stop() noexcept {
        assert(m_state == e_state::running);
        m_state = e_state::stopped;
    }

    void join() noexcept {
        assert(m_thread.joinable());
        assert(m_state == e_state::finished);
        m_thread.join();
    }

    auto finished() const noexcept {
        return m_state == e_state::finished;
    }
};

}
}
}

#endif //GLADIUS_WORKER_H
