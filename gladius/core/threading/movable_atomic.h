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

    movable_atomic(const movable_atomic &o) {
        this->store(o.load());
    }

    movable_atomic(movable_atomic &&o) noexcept : base_type{o.load()} {
    }

    constexpr movable_atomic(T x) noexcept : base_type (x) { }

public:
    movable_atomic &operator=(const movable_atomic &o) {
        this->store(o.load());
        return *this;
    }

    movable_atomic &operator=(movable_atomic &&o) noexcept {
        this->store(o.load());
        return *this;
    }

public:
    T operator=(T x) noexcept {
        this->store(x);
        return x;
    }
};

}
}
}

#endif //GLADIUS_MOVABLE_ATOMIC_H
