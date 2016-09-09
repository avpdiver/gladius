//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_MEMORY_UTILS_H
#define GLADIUS_MEMORY_UTILS_H

namespace gladius {
namespace core {
namespace memory {

bool is_aligned(void *ptr, int ALIGN) {
    return ((unsigned long)ptr & (ALIGN - 1)) == 0;
}

}
}
}

#endif //GLADIUS_MEMORY_UTILS_H
