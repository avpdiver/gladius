//
// Created by avpdiver on 10/28/16.
//

#ifndef GLADIUS_JOB_H
#define GLADIUS_JOB_H

#include <functional>

#include "movable_atomic.h"

namespace gladius {
namespace core {
namespace threading {
    struct c_job {
        COPY_DEFAULT(c_job);
        MOVE_DEFAULT(c_job);

        std::function<void(void)> m_function;
        c_job* m_parent;
        movable_atomic<int32_t> m_unfinished_jobs;
    };
}
}
}


#endif //GLADIUS_JOB_H
