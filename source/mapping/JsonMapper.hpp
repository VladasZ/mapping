//
//  JsonMapper.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 15/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

#include <iostream>

#include "Result.hpp"
#include "Mapper.hpp"
#include "JsonInclude.hpp"
#include "ExceptionCatch.hpp"

#ifdef USING_JSON

namespace mapping {

    using JSON = nlohmann::json;

    class is_json_mapper_checker_base { };

    template <auto& _mapper>
    class JSONMapper : is_json_mapper_checker_base {

    public:

        static constexpr bool check_for_input = false;

        using Mapper = cu::remove_all_t<decltype(_mapper)>;

        static_assert(is_mapper_v<Mapper>);

        static constexpr auto mapper = _mapper;

        template <class Class>
        static void test(const Class& object) {
            auto json = to_json_string(object);
            Log << json;
            auto parsed_object = parse<Class>(json);
            auto new_json = to_json_string(parsed_object);
            Log << new_json;
            assert(json == new_json);
        }

        template <class T>
        static cu::Result<T> try_parse(const std::string& json) {
            try {
                return parse<T>(json);
            }
            catch (...) {
                return {};// what();
            }
        }

        template <class T>
        static T parse(const std::string& json) {
            static_assert(exists<T>());
            return parse_json<T>(JSON::parse(json));
        }

        template <class T>
        static std::string to_json_string(const T& object) {
            static_assert(exists<T>());
            return to_json(object).dump();
        }

        template<class Array>
        static std::string array_to_json_string(const Array& array) {
            using Class = typename Array::value_type;
            static_assert(exists<Class>);
            auto result = JSON::array();
            for (const auto& value : array) {
                result.push_back(to_json(value));
            }
            return result.dump();
        }

        template <class T>
        static JSON to_json(const T& object) {
            static_assert(exists<T>());

            if constexpr (std::is_pointer_v<T>) {
                if (object == nullptr) {
                    return nullptr;
                }
            }

            JSON json;
            properties<T>([&](auto property) {
                using Property = decltype(property);
                if constexpr (Property::is_secure) return;

                if constexpr (Property::ValueInfo::is_map_type) {

                    using spes = typename Property::MapIsNotSupportedYet;

                    using Key = typename Property::Value::key_type;
                    using KeyInfo = cu::TypeInfo<Key>;

                    for (const auto& [key, value] : Property::get_value(object)) {
                        if constexpr (KeyInfo::is_string) {
                            json[property.name()][key] = value;
                        }
                        else if constexpr (KeyInfo::is_integer) {
                            json[property.name()][std::to_string(key)] = value;
                        }
                        else {
                           // static_assert(false, "Invalid map key type.");
                        }
                    }
                }
                else if constexpr (Property::ValueInfo::is_array_type) {
                    using ArrayValue = typename Property::Value::value_type;
                    json[property.name()] = JSON::array();
                    for (const auto& val : Property::get_value(object)) {
                        if constexpr (exists<ArrayValue>()) {
                            json[property.name()].push_back(to_json(val));
                        }
                        else {
                            json[property.name()].push_back(val);
                        }
                    }
                }
                else if constexpr (Property::ValueInfo::is_base_type) {
                    json[property.name()] = Property::get_value(object);
                }
                else {
                    json[property.name()] = to_json(Property::get_value(object));
                }
//                else {
//                    static_assert(false);
//                    //Unknown type
//                }
            });
            return json;
        }

    public:

        template <class T>
        static T parse_json(const JSON& json) {
            static_assert(exists<T>());
            T result = create_empty<T>();
            properties<T>([&](auto property) {
                _extract(result, property, json);
            });
            return result;
        }

    private:

        template <class Property,
                  class Class = typename Property::Class,
                  class Value = typename Property::Value>
        static void _extract(Class& object, const Property& property, const JSON& json) {

            if constexpr (check_for_input) {
                check_input(property.name());
            }

            bool exists_in_json = json.find(property.name()) != json.end();

            if (!exists_in_json) {
                if constexpr (Property::is_optional) {
                    return;
                }
                else if constexpr (Property::is_id) {
                    property.set_value(object, -1);
                    return;
                }
                else {
                    Fatal("Failed to parse JSON for: " + property.to_string());
                }
            }

            JSON json_value = json[property.name()];

            if constexpr (Property::ValueInfo::is_array_type) {
                using ArrayValue = typename Property::Value::value_type;
                auto array = property.get_value(object);
                for (const auto& val : json_value) {
                    if constexpr (exists<ArrayValue>()) {
                        array.push_back(parse_json<ArrayValue>(val));
                    }
                    else {
                        array.push_back(val.get<ArrayValue>());
                    }
                }
                property.set_value(object, array);
            }
            else if constexpr (Property::ValueInfo::is_custom_type) {
                property.set_value(object, parse_json<Value>(json_value));
            }
            else {
                if constexpr (check_for_input && Property::ValueInfo::is_string) {
                    check_input(json_value.get<std::string>());
                }
                property.set_value(object, json_value.get<Value>());
            }
        }

        static void check_input(const std::string& string) {
            static constexpr std::array forbidded_symbols = { '\"', '\'', ':', '.', '=', '*', ';', ',', '?' };
            for (auto ch : string) {
                if (cu::array::contains(forbidded_symbols, ch)) {
                    throw std::runtime_error("Invalid symbols in string: " + string);
                };
            }
        }

    public:

        template <class T>
        static constexpr bool exists() {
            return mapper.template exists<T>();
        }

        template <class T, class Action>
        static constexpr void properties(Action action) {
            mapper.template properties<T>(action);
        }

        template <class T>
        static constexpr T create_empty() {
            return mapper.template create_empty<T>();
        }

        template <class T>
        static void print(const T& object) {
            static_assert(exists<T>);
            std::cout << to_json_string(object) << std::endl;
        }

    };

    template <class T> constexpr bool is_json_mapper_v = std::is_base_of_v<is_json_mapper_checker_base, cu::remove_all_t<T>>;

    inline constexpr JSONMapper<empty_mapper> empty_json_mapper;

}

#endif
