//
// Created by avpdiver on 10/29/16.
//

#ifndef GLADIUS_STATIC_IF_H
#define GLADIUS_STATIC_IF_H

#include <utility>

namespace gladius {
    namespace core {
        namespace meta {

            namespace static_if_detail {

                struct identity {
                    template<typename T>
                    T operator()(T &&x) const {
                        return std::forward<T>(x);
                    }
                };

                template<bool CONDITION>
                struct statement {
                    template<typename F>
                    void then(const F &f) {
                        f(identity());
                    }

                    template<typename F>
                    void else_(const F &) {}
                };

                template<>
                struct statement<false> {
                    template<typename F>
                    void then(const F &) {}

                    template<typename F>
                    void else_(const F &f) {
                        f(identity());
                    }
                };
            }

            template<bool CONDITION, typename F>
            static_if_detail::statement<CONDITION> static_if(F const &f) {
                static_if_detail::statement<CONDITION> if_;
                if_.then(f);
                return if_;
            }

        }
    }
}

#endif //GLADIUS_STATIC_IF_H
