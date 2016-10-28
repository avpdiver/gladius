//
// Created by avpdiver on 10/28/16.
//

#ifndef GLADIUS_SYSTEM_H
#define GLADIUS_SYSTEM_H

#include <type_traits>
#include <vector>
#include <cassert>

namespace gladius { namespace ecs {

        template<typename COMPONENT, std::size_t INITIAL_COMPONENTS_NUMBER, bool DYNAMIC_STORAGE,
                typename... DEPENDS_ON_COMPONENT>
        class c_system {
        private:
            template<bool DYNAMIC>
            struct s_component_storage {
            };

            template<>
            class s_component_storage<true> {
            public:
                s_component_storage() : storage(INITIAL_COMPONENTS_NUMBER) {}

            public:
                COMPONENT& operator[](std::size_t idx) {
                    assert(idx < storage.size());
                    return storage[idx];
                }

                const COMPONENT& operator[](std::size_t idx) const {
                    assert(idx < storage.size());
                    return storage[idx];
                }
            private:
                std::vector<std::aligned_storage<sizeof(COMPONENT), alignof(COMPONENT)>::type> storage;
            };

            class s_component_storage<false> {
            public:
                COMPONENT& operator[](std::size_t idx) {
                    assert(idx < INITIAL_COMPONENTS_NUMBER);
                    return *reinterpret_cast<COMPONENT*>(storage[idx].__data);
                }

                const COMPONENT& operator[](std::size_t idx) const {
                    assert(idx < INITIAL_COMPONENTS_NUMBER);
                    return *reinterpret_cast<const COMPONENT*>(storage[idx].__data);
                }
            private:
                std::aligned_storage<sizeof(COMPONENT), alignof(COMPONENT)>::type storage[INITIAL_COMPONENTS_NUMBER];
            };

            using component_storage_t = s_component_storage<DYNAMIC_STORAGE>;

        private:
            component_storage_t m_component_storage;
        };
}
}

#endif //GLADIUS_SYSTEM_H
