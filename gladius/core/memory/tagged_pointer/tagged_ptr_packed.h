//
// Created by avpdiver on 10/30/16.
//

#ifndef GLADIUS_TAGGED_PTR_PACKED_H
#define GLADIUS_TAGGED_PTR_PACKED_H

#include <cstddef>
#include <limits>
#include <cstdint>

namespace gladius {
    namespace core {
        namespace lockfree {
            template<class T>
            class tagged_ptr {
            private:
                typedef uint64_t packed_ptr_t;

            public:
                typedef uint16_t tag_t;

            private:
                union cast_unit {
                    packed_ptr_t value;
                    tag_t tag[4];
                };

                static const int tag_index = 3;
                static const packed_ptr_t ptr_mask = 0xffffffffffffUL; //(1L<<48L)-1;

                static T *extract_ptr(volatile packed_ptr_t const &i) {
                    return (T *) (i & ptr_mask);
                }

                static tag_t extract_tag(volatile packed_ptr_t const &i) {
                    cast_unit cu;
                    cu.value = i;
                    return cu.tag[tag_index];
                }

                static packed_ptr_t pack_ptr(T *ptr, tag_t tag) {
                    cast_unit ret;
                    ret.value = packed_ptr_t(ptr);
                    ret.tag[tag_index] = tag;
                    return ret.value;
                }

            public:
                tagged_ptr(void) noexcept //: ptr(0), tag(0)
                {}

                tagged_ptr(tagged_ptr const &p) = default;

                explicit tagged_ptr(T *p, tag_t t = 0) :
                        ptr(pack_ptr(p, t)) {}

            public:
                tagged_ptr &operator=(tagged_ptr const &p) = default;

                void set(T *p, tag_t t) {
                    ptr = pack_ptr(p, t);
                }

            public:
                bool operator==(volatile tagged_ptr const &p) const {
                    return (ptr == p.ptr);
                }

                bool operator!=(volatile tagged_ptr const &p) const {
                    return !operator==(p);
                }

            public:
                T *get_ptr() const {
                    return extract_ptr(ptr);
                }

                void set_ptr(T *p) {
                    tag_t tag = get_tag();
                    ptr = pack_ptr(p, tag);
                }

            public:
                tag_t get_tag() const {
                    return extract_tag(ptr);
                }

                tag_t get_next_tag() const {
                    tag_t next = (get_tag() + 1u) & (std::numeric_limits<tag_t>::max)();
                    return next;
                }

                void set_tag(tag_t t) {
                    T *p = get_ptr();
                    ptr = pack_ptr(p, t);
                }

            public:
                T &operator*() const {
                    return *get_ptr();
                }

                T *operator->() const {
                    return get_ptr();
                }

                operator bool(void) const {
                    return get_ptr() != 0;
                }

            protected:
                packed_ptr_t ptr;
            };
        }
    }
}

#endif //GLADIUS_TAGGED_PTR_PACKED_H
