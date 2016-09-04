//
// Created by alexa on 004 04 09 2016.
//

#ifndef GLADIUS_JSON_H
#define GLADIUS_JSON_H

#include <cstddef>
#include <type_traits>
#include <tuple>
#include <json.hpp>

#define JSON_FIELDS constexpr static auto json_fields = std::make_tuple
#define JSON_FIELD(CLASS, FIELD) gladius::core::json::make_json_field(&CLASS::FIELD, #FIELD)


namespace gladius {
namespace core {
namespace json {

enum class e_json_field_type {
    primitive,
    object,
    vector
};

template<typename CLASS, typename TYPE>
struct s_json_field {
    constexpr s_json_field(TYPE CLASS::*member, const char *name, e_json_field_type type)
            : m_member{member}, m_name{name}, m_type{type} {
    }

    using Type = TYPE;

    TYPE CLASS::*m_member;
    const char *m_name;
    e_json_field_type m_type;
};

namespace __impl {

template<typename T>
struct is_vector : public std::false_type {
};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {
};

template<typename T>
struct is_vector_type_class : public std::false_type {
};

template<typename T, typename A>
struct is_vector_type_class<std::vector<T, A>>
        : public std::integral_constant<bool, (std::is_class<T>::value && !std::is_same<std::string, T>::value)> {
};

}

template<typename CLASS, typename TYPE>
constexpr auto make_json_field(TYPE CLASS::*member, const char *name) {
    e_json_field_type type = e_json_field_type::primitive;
    if (std::is_same<std::string, TYPE>::value) {
        type = e_json_field_type::primitive;
    } else if (__impl::is_vector<TYPE>::value) {
        if (__impl::is_vector_type_class<TYPE>::value) {
            type = e_json_field_type::vector;
        } else {
            type = e_json_field_type::primitive;
        }
    } else if (std::is_class<TYPE>::value) {
        type = e_json_field_type::object;
    } else {
        type = e_json_field_type::primitive;
    }
    return s_json_field<CLASS, TYPE>{member, name, type};
}

template<typename CLASS>
void from_json(CLASS &object, const nlohmann::json &data);

namespace __impl {

template<e_json_field_type FIELD, typename CLASS, typename TYPE>
std::enable_if_t<FIELD == e_json_field_type::primitive>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
    object.*(member) = data[name].get<TYPE>();
}

template<e_json_field_type FIELD, typename CLASS, typename TYPE>
std::enable_if_t<FIELD == e_json_field_type::object>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
    from_json(object.*(member), data[name]);
}

template<e_json_field_type FIELD, typename CLASS, typename TYPE>
std::enable_if_t<FIELD == e_json_field_type::vector>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
    auto vector = data[name];
    using VALUE_TYPE = typename TYPE::value_type;
    for (auto& v : vector) {
        VALUE_TYPE value;
        from_json(value, v);
        (object.*(member)).push_back(std::move(value));
    }
    //object.*(member) = data[name].get<TYPE>();
}

// deserialize functions
template<std::size_t I, typename CLASS>
void read_json_field(CLASS &object, const nlohmann::json &data) {
    constexpr auto property = std::get<I>(std::decay_t<CLASS>::json_fields);
    using TYPE = typename decltype(property)::Type;
    read_json_value<property.m_type, CLASS, TYPE>(object, data, property.m_member, property.m_name);
}

template<std::size_t I, typename CLASS>
std::enable_if_t<(I == 0)>
iterate_json_field(CLASS &object, const nlohmann::json &data) {
    read_json_field<I>(object, data);
}

template<std::size_t I, typename CLASS>
std::enable_if_t<(I > 0)>
iterate_json_field(CLASS &object, const nlohmann::json &data) {
    read_json_field<I>(object, data);
    iterate_json_field<I - 1>(object, data);
}
}

template<typename CLASS>
void from_json(CLASS &object, const nlohmann::json &data) {
    __impl::iterate_json_field<std::tuple_size<decltype(std::decay_t<CLASS>::json_fields)>::value - 1>(object, data);
}

}
}
}

#endif //GLADIUS_JSON_H
