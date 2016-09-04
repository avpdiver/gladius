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

template<typename CLASS, typename TYPE>
struct s_json_field {
    constexpr s_json_field(TYPE CLASS::*member, const char *name, bool is_object)
            : m_member{member}, m_name{name}, m_is_object{is_object} {
    }

    using Type = TYPE;

    TYPE CLASS::*m_member;
    const char *m_name;
    bool m_is_object;
};

namespace __impl {
template<typename T>
struct is_vector : public std::false_type {
};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {
};
}

template<typename CLASS, typename TYPE>
constexpr auto make_json_field(TYPE CLASS::*member, const char *name) {
    bool is_object = false;
    if (std::is_same<std::string, TYPE>::value || __impl::is_vector<TYPE>::value) {
        is_object = false;
    } else {
        is_object = std::is_class<TYPE>::value;
    }
    return s_json_field<CLASS, TYPE>{member, name, is_object};
}

template<typename CLASS>
void from_json(CLASS &object, const nlohmann::json &data);

namespace __impl {
template<bool IS_OBJECT, typename CLASS, typename TYPE>
std::enable_if_t<IS_OBJECT>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
    from_json(object.*(member), data[name]);
}

template<bool IS_OBJECT, typename CLASS, typename TYPE>
std::enable_if_t<!IS_OBJECT>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
    object.*(member) = data[name].get<TYPE>();
}

// deserialize functions
template<std::size_t I, typename CLASS>
void read_json_field(CLASS &object, const nlohmann::json &data) {
    constexpr auto property = std::get<I>(std::decay_t<CLASS>::json_fields);
    using TYPE = typename decltype(property)::Type;
    read_json_value<property.m_is_object, CLASS, TYPE>(object, data, property.m_member, property.m_name);
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
