//
// Created by alexa on 22.11.2015.
//

#ifndef GLADIUS_ALIGNED_ALLOCATOR_H
#define GLADIUS_ALIGNED_ALLOCATOR_H

#include <cstddef>
#include <mm_malloc.h>
#include <stdexcept>

namespace gladius {
namespace core {
namespace collections {

template<typename T, size_t ALIGNMENT>
class c_aligned_allocator {
public:
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T &reference;
	typedef const T &const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

public:
	c_aligned_allocator() {
	}

	c_aligned_allocator(const c_aligned_allocator &) {
	}

	template<typename U>
	c_aligned_allocator(const c_aligned_allocator<U, ALIGNMENT> &) {
	}

	~c_aligned_allocator() {
	}

public:
	T *address(T &r) const {
		return &r;
	}

	const T *address(const T &s) const {
		return &s;
	}

	size_t max_size() const {
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	template<typename U>
	struct rebind {
		typedef c_aligned_allocator<U, ALIGNMENT> other;
	};

	bool operator!=(const c_aligned_allocator &other) const {
		return !(*this == other);
	}

	void construct(T *const p, const T &t) const {
		void *const pv = static_cast<void *>(p);

		new(pv) T(t);
	}

	void destroy(T *const p) const {
		p->~T();
	}

	bool operator==(const c_aligned_allocator &other) const {
		return true;
	}

public:
	T *allocate(const size_t n) const {
		if (n == 0) {
			return nullptr;
		}

		if (n > max_size()) {
			throw std::length_error("c_aligned_allocator<T>::allocate() - Integer overflow.");
		}

		// Mallocator wraps malloc().
		void *const pv = _mm_malloc(n * sizeof(T), ALIGNMENT);

		if (pv == nullptr) {
			throw std::bad_alloc();
		}

		return static_cast<T *>(pv);
	}

	void deallocate(T *const p, const std::size_t n) const {
		_mm_free(p);
	}

	template<typename U>
	T *allocate(const std::size_t n, const U * /* const hint */) const {
		return allocate(n);
	}

private:
	c_aligned_allocator &operator=(const c_aligned_allocator &);
};

}
}
}

#endif //GLADIUS_ALIGNED_ALLOCATOR_H
