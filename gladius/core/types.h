//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_TYPES_H
#define GLADIUS_TYPES_H

#include <stddef.h>

#define CTOR(T)							\
	T();

#define DEFAULT(T)						\
	T() = default

#define DEFAULT_COPY(T)                 \
    T(const T&) = default;              \
    T& operator=(const T&) = default

#define COPY(T)                 		\
    T(const T&);               			\
    T& operator=(const T&)

#define DEFAULT_MOVE(T)                 \
    T(T&&) = default;                   \
    T& operator=(T&&) = default

#define MOVE(T)                 		\
    T(T&&);                   			\
    T& operator=(T&&)

#define DEFAULT_MOVE_ONLY(T)            \
    T(const T&) = delete;               \
    T& operator=(const T&) = delete;    \
    T(T&&) = default;                   \
    T& operator=(T&&) = default

#define MOVE_ONLY(T)                    \
    T(const T&) = delete;               \
    T& operator=(const T&) = delete;    \
    T(T&&);                   			\
    T& operator=(T&&)

#define DTOR(T)							\
	~T()


#define DEFAULT_MOVE_IMPL(T)							\
	T::T(T &&o) { *this = std::move(o); }				\
	T& T::operator=(T &&o) {							\
		memcpy(this, &o, sizeof(T));					\
		memset(&o, 0, sizeof(T));						\
		return *this; }

namespace gladius {

typedef size_t handle_t;
static const handle_t INVALID_HANDLE = -1;

}

#endif //GLADIUS_TYPES_H
