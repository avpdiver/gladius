//
// Created by avpdiver on 30.08.16.
//

#ifndef GLADIUS_CONCURRENT_QUEUE_H
#define GLADIUS_CONCURRENT_QUEUE_H

#include <cstddef>

namespace gladius {
namespace core {
namespace collections {

    template<typename T, size_t CAPACITY>
    class c_concurrent_queue {

    public:
        void push(const T& value) {

        }

        bool pop(T& value) {
            return true;
        }
    };
}
}
}
#endif //GLADIUS_CONCURRENT_QUEUE_H
