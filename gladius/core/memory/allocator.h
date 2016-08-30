//
// Created by alexa on 023 23 02 2016.
//

#ifndef GLADIUS_ALLOCATOR_H
#define GLADIUS_ALLOCATOR_H

#include <memory>
#include <cstddef>
#include <stdint.h>

namespace gladius {
namespace core {
namespace memory {

template<class ALLOC_POLICY, class THREAD_POLICY, class BOUNDS_POLICY, class TRACKING_POLICY, class TAGGING_POLICY>
class c_allocator {
public:
    c_allocator(size_t size, void *start)
        : m_allocator(size, start) {
    }

public:
    void *alloc(size_t size, size_t align, size_t offset = 0) {
        m_thread_guard.lock();

        const size_t original_size = size;
        const size_t new_size = BOUNDS_POLICY::SIZE_FRONT + offset + size + BOUNDS_POLICY::SIZE_BACK;
        char *ptr = static_cast<char *>(m_allocator.alloc(new_size, align, BOUNDS_POLICY::SIZE_FRONT + offset));

        m_bounds_checker.guard_front(ptr);
        m_bounds_checker.guard_back(ptr + BOUNDS_POLICY::SIZE_FRONT + offset + original_size);
        m_tagger.alloc(ptr + BOUNDS_POLICY::SIZE_FRONT, original_size);
        m_tracker.alloc(ptr, new_size, align);

        m_thread_guard.unlock();

        return (ptr + BOUNDS_POLICY::SIZE_FRONT);
    }

    void free(void *ptr) {
        m_thread_guard.lock();

        char *original = static_cast<char *>(ptr) - BOUNDS_POLICY::SIZE_FRONT;
        const size_t size = m_allocator.get_size(original);

        m_bounds_checker.check_front(original);
        m_bounds_checker.check_back(original + size - BOUNDS_POLICY::SIZE_BACK);
        m_tracker.free(original);
        m_tagger.free(original, size);
        m_allocator.free(original);

        m_thread_guard.unlock();
    }

private:
    ALLOC_POLICY m_allocator;
    THREAD_POLICY m_thread_guard;
    BOUNDS_POLICY m_bounds_checker;
    TRACKING_POLICY m_tracker;
    TAGGING_POLICY m_tagger;
};

template<class T>
T *alloc_new(c_allocator &allocator) {
    return new(allocator.alloc(sizeof(T), __alignof(T))) T;
}

template<class T>
T *alloc_new(c_allocator &allocator, const T &t) {
    return new(allocator.alloc(sizeof(T), __alignof(T))) T(t);
}

template<class T>
void free_delete(c_allocator &allocator, T &object) {
    object.~T();
    allocator.free(&object);
}

template<class T>
T *alloc_array(c_allocator &allocator, size_t length) {
    uint8_t headerSize = sizeof(size_t) / sizeof(T);

    if (sizeof(size_t) % sizeof(T) > 0) {
        headerSize += 1;
    }

    //Allocate extra space to store array length in the bytes before the array
    T *p = ((T *) allocator.alloc(sizeof(T) * (length + headerSize), __alignof(T))) + headerSize;

    *(((size_t *) p) - 1) = length;

    for (size_t i = 0; i < length; i++) {
        new(&p[i]) T;
    }

    return p;
}

template<class T>
void free_array(c_allocator &allocator, T *array) {
    size_t length = *(((size_t *) array) - 1);
    for (size_t i = 0; i < length; i++) {
        array[i].~T();
    }

    //Calculate how much extra memory was allocated to store the length before the array
    uint8_t headerSize = sizeof(size_t) / sizeof(T);

    if (sizeof(size_t) % sizeof(T) > 0) {
        headerSize += 1;
    }

    allocator.free(array - headerSize);
}

}
}
}

#endif //GLADIUS_ALLOCATOR_H
