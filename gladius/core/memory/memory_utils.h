//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_MEMORY_UTILS_H
#define GLADIUS_MEMORY_UTILS_H

namespace gladius {
namespace core {
namespace memory {

inline bool is_aligned(void *ptr, int ALIGN) {
    return (reinterpret_cast<size_t>(ptr) & (ALIGN - 1)) == 0;
}

}
}
}

#endif //GLADIUS_MEMORY_UTILS_H
