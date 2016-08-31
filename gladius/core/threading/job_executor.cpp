//
// Created by pav on 11.09.15.
//

#include <thread>
#include <queue>
#include "job_executor.h"
#include "../collections/steal_queue.h"


namespace gladius {
namespace core {
namespace threading {
namespace c_job_executor {

static const size_t JOB_COUNT = 1024;

struct s_thread_context {
public:
    std::thread *m_thread;
    collections::c_steal_queue<c_job*, JOB_COUNT> m_jobs_queue;
    c_job m_job_pool[JOB_COUNT];
    size_t m_jobs_counter = 0;

public:
    c_job *get_job() {
        return &m_job_pool[m_jobs_counter++ & (JOB_COUNT - 1)];
    }
};

s_thread_context *g_thread_contexts;
size_t g_processors_count;
std::atomic_int g_running;

c_job *g_jobs_pool[JOB_COUNT];
std::atomic_size_t g_jobs_counter;

thread_local size_t g_thread_index;

bool is_running(const c_job *const job) {
    size_t u = job->m_unfinished_jobs.load();
    return u > 0;
}

void finish(c_job *job) {
    size_t count = job->m_unfinished_jobs.fetch_sub(1);
    if (job->m_parent != nullptr) {
        job->m_parent->m_unfinished_jobs.fetch_sub(1);
    }
    if (count > 1) {
        return;
    }
    g_jobs_counter.fetch_sub(1);
}

void execute(size_t thread_index) {
    g_thread_index = thread_index;
    auto &thread_context = g_thread_contexts[g_thread_index];

    c_job *job;
    while (g_running.load(std::memory_order_relaxed) == 1) {
        if (!thread_context.m_jobs_queue.pop(job)) {
            size_t index = std::rand() % g_processors_count;
            if (index == thread_index || !g_thread_contexts[index].m_jobs_queue.steal(job)) {
                std::this_thread::yield();
                continue;
            }
        }
        if (job->m_function != nullptr) {
            (*job->m_function)(job->m_data);
        }
        finish(job);
    }
}

bool init() {
    g_jobs_counter.store(0);
    g_processors_count = std::max(std::thread::hardware_concurrency(), 10u);
    g_thread_contexts = new s_thread_context[g_processors_count];
    g_running.store(1);

    g_thread_contexts[0].m_thread = nullptr;
    g_thread_index = 0;

    for (size_t i = 1; i < g_processors_count; i++) {
        g_thread_contexts[i].m_thread = new std::thread(execute, i);
    }

    return true;
}

void release() {
    g_running.store(0);
    for (size_t i = 1; i < g_processors_count; i++) {
        g_thread_contexts[i].m_thread->join();
        delete g_thread_contexts[i].m_thread;
    }

    delete g_thread_contexts;
}

job_handle_t create_job(job_function_t function, void *data, job_handle_t parent) {
    size_t count;
    if ((count = g_jobs_counter.fetch_add(1)) >= JOB_COUNT) {
        g_jobs_counter.store(JOB_COUNT);
        return INVALID_HANDLE;
    }

    job_handle_t handle = count & (JOB_COUNT - 1);
    c_job *job = g_thread_contexts[g_thread_index].get_job();
    job->m_function = function;
    job->m_data = data;
    job->m_unfinished_jobs.store(1);
    if (parent != INVALID_HANDLE) {
        job->m_parent = g_jobs_pool[parent];
        job->m_parent->m_unfinished_jobs.fetch_add(1);
    } else {
        job->m_parent = nullptr;
    }

    g_jobs_pool[handle] = job;
    return handle;
}

job_handle_t create_and_execunte_job(job_function_t function, void *data, job_handle_t parent) {
    job_handle_t job = create_job(function, data, parent);
    execute_job(job);
    return job;
}

void execute_job(job_handle_t handle) {
    c_job *job = g_jobs_pool[handle];
    g_thread_contexts[g_thread_index].m_jobs_queue.push(job);
}

void wait(job_handle_t handle) {
    c_job *job = g_jobs_pool[handle];
    while (is_running(job)) {
        std::this_thread::yield();
    }
}

}
}
}
}