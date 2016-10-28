//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_WORKER_POOL_H
#define GLADIUS_WORKER_POOL_H

#include <atomic>

#include "../collections/concurrent_queue.h"

#include "worker.h"


namespace gladius {
namespace core {
namespace threading {

class c_worker_pool {
private:


private:
    task_queue_t m_queue;
    std::vector <c_worker> m_workers;
    std::atomic <size_t> m_remaining_inits;

public:
    c_worker_pool() {
        auto hwc = std::thread::hardware_concurrency();
        auto worker_count = hwc;
        initialize_workers(worker_count);
    }

    ~c_worker_pool() {
        // Busy wait for all workers to be initialized.
        while (m_remaining_inits > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Signal all workers to exit their processing loops.
        for (auto &w : m_workers) {
            w.stop();
        }

        // Post dummy tasks until all workers have exited their loops.
        while (!all_workers_finished()) {
            push({});
        }

        // Join the workers' threads.
        for (auto &w : m_workers) {
            w.join();
        }
    }

public:
    void push(c_job&& f) {
        m_queue.push(f);
    }
    
private:

    void initialize_workers(size_t n) {
        // Create workers.
        for (size_t i = 0; i < n; ++i) {
            m_workers.emplace_back(&m_queue);
        }

        // Start workers.
        m_remaining_inits = n;
        for (auto &w : m_workers) {
            w.start(m_remaining_inits);
        }
    }

    bool all_workers_finished() const noexcept {
        for (const auto &w : m_workers) {
            if (!w.finished()) {
                return false;
            }
        }

        return true;
    }
};

}
}
}

#endif //GLADIUS_WORKER_POOL_H
