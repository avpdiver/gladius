//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_TYPES_H
#define GLADIUS_TYPES_H

#include <stddef.h>

#define CTOR_COPY(T)            T(const T&)
#define CTOR_COPY_DEFAULT(T)    T(const T&) = default
#define NO_CTOR_COPY(T)         T(const T&) = delete
#define COPY(T)                 T& operator=(const T&)
#define COPY_DEFAULT(T)         T& operator=(const T&) = default
#define NO_COPY(T)              T& operator=(const T&) = delete
#define COPYABLE(T)				CTOR_COPY(T); COPY(T)
#define COPYABLE_DEFAULT(T)		CTOR_COPY_DEFAULT(T); COPY_DEFAULT(T)
#define NOT_COPYABLE(T)			NO_CTOR_COPY(T); NO_COPY(T)

#define CTOR_MOVE(T)            T(T&&)
#define CTOR_MOVE_DEFAULT(T)    T(T&&) = default
#define NO_CTOR_MOVE(T)         T(T&&) = delete
#define MOVE(T)                 T& operator=(T&&)
#define MOVE_DEFAULT(T)         T& operator=(T&&) = default
#define NO_MOVE(T)              T& operator=(T&&) = delete
#define MOVEABLE(T)				CTOR_MOVE(T); MOVE(T)
#define MOVEABLE_DEFAULT(T)		CTOR_MOVE_DEFAULT(T); MOVE_DEFAULT(T)
#define NOT_MOVEABLE(T)			NO_CTOR_MOVE(T); NO_MOVE(T)


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
