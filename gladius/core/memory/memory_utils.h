//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_MEMORY_UTILS_H
#define GLADIUS_MEMORY_UTILS_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace gladius {
	namespace core {
		namespace memory {
			template<typename T>
			static inline char *align_for(char *ptr) {
				const std::size_t alignment = std::alignment_of<T>::value;
				return ptr + (alignment - (reinterpret_cast<std::uintptr_t>(ptr) % alignment)) % alignment;
			}
		}
	}
}

#endif //GLADIUS_MEMORY_UTILS_H
