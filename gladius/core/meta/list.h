//
// Created by avpdiver on 10/29/16.
//

#ifndef GLADIUS_LIST_H
#define GLADIUS_LIST_H

#include <type_traits>

namespace gladius {
    namespace core {
        namespace meta {

            struct nil {
            };

            template<typename HEAD, typename TAIL=nil>
            struct list {
                typedef HEAD head;
                typedef TAIL tail;
            };

            namespace list {
                template<typename LIST>
                struct length {
                    static const std::size_t value = 1 + length<typename LIST::tail>::value;
                };

                template<>
                struct length<nil> {
                    static const std::size_t value = 0;
                };

                template<typename LIST, std::size_t N>
                struct nth {
                    typedef typename LIST::tail tail;
                    typedef typename nth<tail, N - 1>::value value;
                };

                template<typename LIST>
                struct nth<LIST, 0> {
                    typedef typename LIST::head value;
                };

                template <class TYPE, class LIST> struct contains_type {
                    static const bool value = std::is_same<TYPE, typename LIST::head>::value ||
                                               contains_type<TYPE, typename LIST::tail>::value;
                };

                template <class TYPE> struct contains_type<TYPE, nil> {
                    static const bool value = false;
                };

                template <typename ELEMENT, typename LIST=nil> struct prepend {
                    typedef list<ELEMENT, LIST> value;
                };

                template <class ELEMENT, class LIST> struct append {
                    typedef typename LIST::head head;
                    typedef typename LIST::tail tail;

                    typedef typename append<ELEMENT, tail>::value next;
                    typedef list<head, next> value;
                };

                template <class ELEMENT> struct append<ELEMENT, nil> {
                    typedef list<ELEMENT> value;
                };
            }
        }
    }
}

#endif //GLADIUS_LIST_H
