//
// Created by pav on 11.09.15.
//

#ifndef GLADIUS_JOBEXECUTOR_H
#define GLADIUS_JOBEXECUTOR_H

#include "job.h"

namespace gladius {
namespace core {
namespace threading {

namespace c_job_executor {
bool init();
void release();

job_handle_t create_job(job_function_t function = nullptr, void *data = nullptr, job_handle_t parent = INVALID_HANDLE);
job_handle_t create_and_execunte_job(job_function_t function = nullptr,
                                     void *data = nullptr,
                                     job_handle_t parent = INVALID_HANDLE);
void execute_job(job_handle_t handle);
void wait(job_handle_t job);
}

}
}
}

#endif //GLADIUS_JOBEXECUTOR_H
