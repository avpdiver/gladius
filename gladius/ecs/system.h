//
// Created by avpdiver on 10/28/16.
//

#ifndef GLADIUS_SYSTEM_H
#define GLADIUS_SYSTEM_H

#include <type_traits>
#include <vector>
#include <cassert>
#include "../core/meta/minmax.h"
#include "../core/threading/worker_pool.h"

namespace gladius {
namespace ecs {


template<typename...>
struct system_dependency {
    static constexpr size_t value = 0;
};

template<typename SYSTEM, typename... DEPENDENCIES>
struct system_dependency<SYSTEM, DEPENDENCIES...> {
    static constexpr size_t value = 1 + core::meta::max<size_t, 0, system_dependency<DEPENDENCIES...>::value>::value;
};

template<typename SYSTEM, size_t THREADS, typename... DEPENDS_ON>
struct ecs_system {
    static constexpr size_t dependency_level = system_dependency<DEPENDS_ON...>::value;
    static SYSTEM system;
    static void execute() {
        system.process();
    }
};

template<typename STORAGE, typename... SYSTEMS>
struct ecs {
private:
    static core::threading::c_worker_pool worker_pool;

public:
    static void execute() {

    }
};


}
}

#endif //GLADIUS_SYSTEM_H
