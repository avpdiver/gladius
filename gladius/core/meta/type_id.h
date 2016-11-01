//
// Created by avpdiver on 11/1/16.
//

#ifndef GLADIUS_TYPE_ID_H
#define GLADIUS_TYPE_ID_H

#include <utility>
#include <cstddef>
#include <tuple>

namespace gladius {
    namespace core {
        namespace meta {
            template <typename TYPE>
            struct s_type_id {
                static const TYPE* id;
            };

            template <typename TYPE>
            const TYPE* s_type_id<TYPE>::id = nullptr;

            template <typename TYPE>
            constexpr size_t type_id() {
                return (const size_t)(&(s_type_id<TYPE>::id));
            }

        }
    }
}

#endif //GLADIUS_TYPE_ID_H
