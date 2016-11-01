//
// Created by avpdiver on 10/28/16.
//

#ifndef GLADIUS_SYSTEM_H
#define GLADIUS_SYSTEM_H

#include <type_traits>
#include <vector>
#include <cassert>
#include "../core/meta/list.h"

namespace gladius {
    namespace ecs {
        template<typename SYSTEM, typename... DEPENDS_ON>
        class c_system {
        public:
            static constexpr auto dependency_level = core::meta::type_list_length<typename core::meta::make_list<DEPENDS_ON...>::type>::value;
        };
    }
}

#endif //GLADIUS_SYSTEM_H
