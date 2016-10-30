//
// Created by avpdiver on 10/30/16.
//

#ifndef GLADIUS_TAGGED_PTR_DCAS_H
#define GLADIUS_TAGGED_PTR_DCAS_H

#include <cstddef>
#include <limits>

namespace gladius {
    namespace core {
        namespace lockfree {
            template<class T>
            class alignas(2 * sizeof(void *)) tagged_ptr {
            public:
                typedef std::size_t tag_t;

            public:
                tagged_ptr() noexcept {}

                tagged_ptr(tagged_ptr const &p) = default;

                explicit tagged_ptr(T *p, tag_t t = 0) :
                        ptr(p), tag(t) {}

            public:
                tagged_ptr &operator=(tagged_ptr const &p) = default;

                void set(T *p, tag_t t) {
                    ptr = p;
                    tag = t;
                }

            public:
                bool operator==(volatile tagged_ptr const &p) const {
                    return (ptr == p.ptr) && (tag == p.tag);
                }

                bool operator!=(volatile tagged_ptr const &p) const {
                    return !operator==(p);
                }

            public:
                T *get_ptr(void) const {
                    return ptr;
                }

                void set_ptr(T *p) {
                    ptr = p;
                }

            public:
                tag_t get_tag() const {
                    return tag;
                }

                tag_t get_next_tag() const {
                    tag_t next = (get_tag() + 1) & (std::numeric_limits<tag_t>::max)();
                    return next;
                }

                void set_tag(tag_t t) {
                    tag = t;
                }

            public:
                T &operator*() const {
                    return *ptr;
                }

                T *operator->() const {
                    return ptr;
                }

                operator bool(void) const {
                    return ptr != 0;
                }

            protected:
                T *ptr;
                tag_t tag;
            };
        }
    }
}

#endif //GLADIUS_TAGGED_PTR_DCAS_H
