//
// Created by alexa on 001 01 11 2016.
//

#ifndef GLADIUS_MINMAX_H
#define GLADIUS_MINMAX_H

#include <cstddef>
#include <limits>

namespace gladius {
namespace core {
namespace meta {

template <typename T, T...>
struct max {
    static constexpr auto value = std::numeric_limits<T>::min();
};

template <typename T, T h, T... v>
struct max<T, h, v...> {
private:
    static constexpr auto prev = max<T, v...>::value;
public:
    static constexpr auto value = h > prev ? h : prev;
};


template <typename T, T...>
struct min {
    static constexpr auto value = std::numeric_limits<T>::max();
};

template <typename T, T h, T... v>
struct min<T, h, v...> {
private:
    static constexpr auto prev = min<T, v...>::value;
public:
    static constexpr auto value = h < prev ? h : prev;
};

}
}
}

#endif //GLADIUS_MINMAX_H
