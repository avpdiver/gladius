//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_TYPES_H
#define GLADIUS_TYPES_H

#include <stddef.h>

#define MOVE_ONLY(T)                    \
    T(const T&) = delete;               \
    T& operator=(const T&) = delete;    \
    T(T&&) = default;                   \
    T& operator=(T&&) = default

namespace gladius {

typedef size_t handle_t;
static const handle_t INVALID_HANDLE = -1;

}

#endif //GLADIUS_TYPES_H
