//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_MOVABLE_ATOMIC_H
#define GLADIUS_MOVABLE_ATOMIC_H

#include <atomic>
#include <type_traits>

namespace gladius {
namespace core {
namespace threading {

template<typename T>
class movable_atomic final : public std::atomic<T> {
private:
    using base_type = std::atomic<T>;

public:
    movable_atomic() = default;

    movable_atomic(const movable_atomic &) = delete;

    movable_atomic &operator=(const movable_atomic &) = delete;

    constexpr movable_atomic(T x) noexcept : base_type (x) { }

    movable_atomic(movable_atomic &&rhs) noexcept : base_type{rhs.load()} {
    }

    movable_atomic &operator=(movable_atomic &&rhs) noexcept {
        this->store(rhs.load());
        return *this;
    }

    T operator=(T x) noexcept {
        this->store(x);
        return x;
    }
};

}
}
}

#endif //GLADIUS_MOVABLE_ATOMIC_H
