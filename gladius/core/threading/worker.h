//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_WORKER_H
#define GLADIUS_WORKER_H

#include <thread>
#include <cassert>

#include "../meta/fixed_function.h"
#include "../collections/concurrent_queue.h"

#include "movable_atomic.h"

namespace gladius {
namespace core {
namespace threading {

    using task_queue_t = collections::c_concurrent_queue<std::function<void(void)>, 4096>;

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
        task_queue_t *m_queue;
        movable_atomic<e_state> m_state{e_state::uninitialized};

    private:

        void run() {
            assert(m_state == e_state::running);

            std::function<void(void)> job;
            while (m_state == e_state::running) {
                if (!m_queue->pop(job)) {
                    continue;
                }
                if (!job) {
                    break;
                }
                job();
            }
            m_state = e_state::finished;
        }

    public:
        c_worker(task_queue_t *queue) noexcept : m_queue{queue} {}

    public:
        NOT_COPYABLE(c_worker);
        MOVEABLE_DEFAULT(c_worker);

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
