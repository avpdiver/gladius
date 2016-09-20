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
#define JSON_FIELD_CONV(CLASS, FIELD, CONV) gladius::core::json::make_json_field(&CLASS::FIELD, #FIELD, CONV)

namespace gladius {
namespace core {
namespace json {

enum class e_json_field_type {
	primitive,
	object,
	vector
};

template<typename CLASS, typename TYPE, typename CONV_OUT, typename CONV_IN>
struct s_json_field {
	constexpr s_json_field(TYPE CLASS::*member, const char *name, e_json_field_type type)
		: m_member {member}, m_name {name}, m_type {type}, m_converter {nullptr} {
	}

	constexpr s_json_field(TYPE CLASS::*member, const char *name, e_json_field_type type,
						   CONV_OUT (*converter)(const CONV_IN &))
		: m_member {member}, m_name {name}, m_type {type}, m_converter {converter} {
	}
	using Type = TYPE;
	using InitialType = CONV_IN;
	using ConvertedType = CONV_OUT;

	TYPE CLASS::*m_member;
	const char *m_name;
	e_json_field_type m_type;
	ConvertedType (*m_converter)(const InitialType &);
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

template<typename T>
struct vector_value_t {
	using type = T;
};

template<typename T, typename A>
struct vector_value_t<std::vector<T, A>> {
	using type = T;
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
	return s_json_field<CLASS, TYPE, typename __impl::vector_value_t<TYPE>::type, typename __impl::vector_value_t<TYPE>::type>{member, name, type};
}

template<typename CLASS, typename TYPE, typename CONV_OUT, typename CONV_IN>
constexpr auto make_json_field(TYPE CLASS::*member, const char *name, CONV_OUT (*converter)(const CONV_IN &)) {
	e_json_field_type type = e_json_field_type::primitive;
	if (std::is_same<std::string, CONV_IN>::value) {
		type = e_json_field_type::primitive;
	} else if (__impl::is_vector<CONV_IN>::value) {
		type = e_json_field_type::vector;
	} else if (std::is_class<CONV_IN>::value) {
		type = e_json_field_type::object;
	} else {
		type = e_json_field_type::primitive;
	}
	return s_json_field<
		CLASS,
		TYPE,
		typename __impl::vector_value_t<CONV_OUT>::type,
		typename __impl::vector_value_t<CONV_IN>::type
	> {member, name, type, converter};
}

template<typename CLASS>
void from_json(CLASS &object, const nlohmann::json &data);

namespace __impl {

// ***********  READ PRIMITIVE VALUE *********************************************************************************************
template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::primitive && CONVERTER == nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	object.*(member) = data[name].get<TYPE>();
}

template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::primitive && CONVERTER != nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	auto value = data[name].get<CONV_IN>();
	object.*(member) = CONVERTER(value);
}

// ***********  READ OBJECT VALUE *********************************************************************************************
template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::object && CONVERTER == nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	from_json(object.*(member), data[name]);
}

template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::object && CONVERTER != nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	CONV_IN value;
	from_json(value, data[name]);
	object.*(member) = CONVERTER(value);
}

// ***********  READ VECTOR VALUE *********************************************************************************************
template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::vector && CONVERTER == nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	auto vector = data[name];
	for (auto &v : vector) {
		CONV_OUT value;
		from_json(value, v);
		(object.*(member)).push_back(std::move(value));
	}
}

template<e_json_field_type FIELD, typename CLASS, typename TYPE,
	typename CONV_OUT, typename CONV_IN, CONV_OUT (*CONVERTER)(const CONV_IN &)>
std::enable_if_t<(FIELD == e_json_field_type::vector && CONVERTER != nullptr)>
read_json_value(CLASS &object, const nlohmann::json &data, TYPE CLASS::*member, const char *name) {
	auto vector = data[name];
	for (auto &v : vector) {
		CONV_IN value;
		from_json(value, v);
		(object.*(member)).push_back(std::move(CONVERTER(value)));
	}
}

// deserialize functions
template<std::size_t I, typename CLASS>
void read_json_field(CLASS &object, const nlohmann::json &data) {
	constexpr auto property = std::get<I>(std::decay_t<CLASS>::json_fields);
	using TYPE = typename decltype(property)::Type;
	using INITIAL_TYPE = typename decltype(property)::InitialType;
	using CONVERTED_TYPE = typename decltype(property)::ConvertedType;
	read_json_value<property.m_type, CLASS, TYPE, CONVERTED_TYPE, INITIAL_TYPE, property.m_converter>(
		object, data, property.m_member, property.m_name);
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
