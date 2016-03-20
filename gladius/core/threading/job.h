//
// Created by pav on 11.09.15.
//

#ifndef GLADIUS_JOB_H
#define GLADIUS_JOB_H

#include <atomic>
#include <stddef.h>
#include "../types.h"

namespace gladius
{
    namespace core
    {
        namespace threading
        {
            typedef handle_t job_handle_t;

            struct c_job;

            typedef void (*job_function_t) (void *);

            struct c_job
            {
                job_function_t m_function;
                c_job *m_parent;
                std::atomic_size_t m_unfinished_jobs;
                void* m_data;
            };
        }
    }
}

#endif //GLADIUS_JOB_H
