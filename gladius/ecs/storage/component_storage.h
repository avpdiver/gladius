//
// Created by avpdiver on 11/1/16.
//

#ifndef GLADIUS_COMPONENT_STORAGE_H
#define GLADIUS_COMPONENT_STORAGE_H

#include <cstddef>
#include <cassert>
#include <gli/type.hpp>
#include <vector>

namespace gladius {
    namespace ecs {

        template<typename COMPONENT, std::size_t INITIAL_ENTITY_NUMBER = 0>
        class s_dynamic_component_storage {
        public:
            s_dynamic_component_storage() : storage(INITIAL_ENTITY_NUMBER) {}

        public:
            COMPONENT &operator[](std::size_t idx) {
                if (idx >= storage.size()) {
                    storage.resize(idx + 1);
                }
                return storage[idx];
            }

            const COMPONENT &operator[](std::size_t idx) const {
                assert(idx < storage.size());
                return storage[idx];
            }

        private:
            std::vector <std::aligned_storage<sizeof(COMPONENT), alignof(COMPONENT)>::type> storage;
        };

        template<typename COMPONENT, std::size_t ENTITY_NUMBER>
        class s_fixed_component_storage {
        public:
            COMPONENT &operator[](std::size_t idx) {
                assert(idx < ENTITY_NUMBER);
                return *reinterpret_cast<COMPONENT *>(storage[idx].__data);
            }

            const COMPONENT &operator[](std::size_t idx) const {
                assert(idx < ENTITY_NUMBER);
                return *reinterpret_cast<const COMPONENT *>(storage[idx].__data);
            }

        private:
            std::aligned_storage<sizeof(COMPONENT), alignof(COMPONENT)>::type storage[ENTITY_NUMBER];
        };
    }
}
#endif //GLADIUS_COMPONENT_STORAGE_H
