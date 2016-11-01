//
// Created by avpdiver on 10/28/16.
//

#ifndef GLADIUS_SYSTEM_H
#define GLADIUS_SYSTEM_H

#include <type_traits>
#include <vector>
#include <cassert>

namespace gladius { namespace ecs {

        template<typename COMPONENT, typename STORAGE, typename... DEPENDS_ON>
        class c_system {
        private:
            using component_storage_t = STORAGE;
            using depends_on_t = std::tuple<DEPENDS_ON...>;

        private:
            constexpr depends_on_t m_depends_on()
        private:
            component_storage_t m_component_storage;
        };
}
}

#endif //GLADIUS_SYSTEM_H
