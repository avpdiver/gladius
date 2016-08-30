//
// Created by avpdiver on 29.08.16.
//

#ifndef GLADIUS_MATH_FUNC_H
#define GLADIUS_MATH_FUNC_H

#include <cstddef>

namespace gladius {
namespace math {

size_t ceil_to_power_2(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    for (size_t i = 1; i < sizeof(size_t); i <<= 1) {
        x |= x >> (i << 3);
    }
    ++x;
    return x;
}

}
}

#endif //GLADIUS_MATH_FUNC_H
