//
// Created by avpdiver on 11/1/16.
//

#ifndef GLADIUS_LIST_H
#define GLADIUS_LIST_H

#include <cstddef>

namespace gladius {
    namespace core {
        namespace meta {

            struct nil_type {
            };

            template<typename HEAD, typename TAIL = nil_type>
            struct type_list {
                using head = HEAD;
                using tail = TAIL;
            };

            template<typename...>
            struct make_list {
                using type = nil_type;
            };

            template<typename HEAD, typename... TAIL>
            struct make_list<HEAD, TAIL...> {
                using type = type_list<HEAD, typename make_list<TAIL...>::type>;
            };


            //------------ list length -----------------------------------------------------------------
            template<typename LIST>
            struct type_list_length {
                static const std::size_t value = 1 + type_list_length<typename LIST::tail>::value;
            };

            template<>
            struct type_list_length<nil_type> {
                static const std::size_t value = 0;
            };

        }
    }
}

#endif //GLADIUS_LIST_H
