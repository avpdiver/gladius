//
// Created by alexa on 018 18 09 2016.
//

#ifndef GLADIUS_FIXED_FUNCTION_H
#define GLADIUS_FIXED_FUNCTION_H

namespace gladius {
namespace core {

template<typename SIGNATURE, size_t STORAGE_SIZE = 64>
class fixed_function;

template<typename RETURN_TYPE, typename... ARGS, size_t STORAGE_SIZE>
class fixed_function<RETURN_TYPE(ARGS...), STORAGE_SIZE> {
private:
    static constexpr auto storage_size = STORAGE_SIZE;

    using ret_type = RETURN_TYPE;
    using storage_type = std::aligned_storage_t<storage_size, alignof(size_t)>;
    using fn_ptr_type = ret_type (*)(ARGS...);
    using method_type = ret_type (*)(storage_type *, fn_ptr_type, ARGS...);
    using alloc_type = void (*)(storage_type *, void *object_ptr);

private:
    union {
        storage_type m_storage;
        fn_ptr_type m_function_ptr;
    };
    method_type m_method_ptr;
    alloc_type m_alloc_ptr;

    void move_from_other(fixed_function &o) noexcept {
        assert(this != &o);

        if (m_alloc_ptr) {
            m_alloc_ptr(&m_storage, nullptr);
            m_alloc_ptr = nullptr;
        } else {
            m_function_ptr = nullptr;
        }

        m_method_ptr = o.m_method_ptr;
        o.m_method_ptr = nullptr;

        if (o.m_alloc_ptr) {
            m_alloc_ptr = o.m_alloc_ptr;
            m_alloc_ptr(&m_storage, &o.m_storage);
        } else {
            m_function_ptr = o.m_function_ptr;
        }
    }

public:
    fixed_function() noexcept : m_function_ptr{nullptr},
                                m_method_ptr{nullptr},
                                m_alloc_ptr{nullptr} {
    }

    fixed_function(const fixed_function &) = delete;

    fixed_function(fixed_function &&o) noexcept : fixed_function() {
        move_from_other(o);
    }

    template<typename R, typename... A>
    fixed_function(R (*f)(A...)) noexcept : fixed_function() {
        m_function_ptr = f;
        m_method_ptr = [](storage_type *, fn_ptr_type xf, ARGS... xs) {
            return static_cast<decltype(f)>(xf)(xs...);
        };
    }

    template<typename TF>
    fixed_function(TF &&f) noexcept : fixed_function() {
        using unref_type = std::remove_reference_t<TF>;

        assert(sizeof(unref_type) < storage_size);
        assert(std::is_move_constructible<unref_type>{});

        m_method_ptr = [](storage_type *s, fn_ptr_type, ARGS... xs) {
            return reinterpret_cast<unref_type*>(s)->operator()(xs...);
        };

        m_alloc_ptr = [](storage_type *s, void *o) {
            if (o) {
                new(s) unref_type(std::move(*static_cast<unref_type*>(o)));
            } else {
                reinterpret_cast<unref_type*>(s)->~unref_type();
            }
        };

        m_alloc_ptr(&m_storage, &f);
    }

    ~fixed_function() noexcept {
        if (m_alloc_ptr) m_alloc_ptr(&m_storage, nullptr);
    }

public:
    fixed_function &operator=(const fixed_function &) = delete;

    fixed_function &operator=(fixed_function &&o) noexcept {
        move_from_other(o);
        return *this;
    }

public:
    /**
     * @brief operator () Execute stored functional object.
     */
    template<typename... TFwdTs>
    auto operator()(TFwdTs &&... xs) noexcept(noexcept(m_method_ptr(&m_storage, m_function_ptr, FWD(xs)...))) {
        assert(m_method_ptr != nullptr);
        return m_method_ptr(&m_storage, m_function_ptr, FWD(xs)...);
    }
};

}
}

#endif //GLADIUS_FIXED_FUNCTION_H
