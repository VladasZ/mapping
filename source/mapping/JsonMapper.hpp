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

    class is_json_mapper_cheker_base { };

    template <auto& _mapper>
    class JSONMapper : is_json_mapper_cheker_base {

    public:

        using Mapper = cu::remove_all_t<decltype(_mapper)>;

        static constexpr bool check_for_input = false;

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

        template <class Class>
        static cu::Result<Class> try_parse(const std::string& json) {
            try {
                return parse<Class>(json);
            }
            catch (...) {
                return {};// what();
            }
        }

        template <class Class>
        static Class parse(const std::string& json) {
            static_assert(exists<Class>);
            return parse_json<Class>(JSON::parse(json));
        }

        template <class Class>
        static std::string to_json_string(const Class& object) {
            static_assert(exists<Class>);
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

        template <class Class>
        static JSON to_json(const Class& object) {
            static_assert(exists<Class>);

            if constexpr (std::is_pointer_v<Class>) {
                if (object == nullptr) {
                    return nullptr;
                }
            }

            JSON json;
            mapper.template iterate_properties<Class>([&](const auto& property) {
                using Property = cu::remove_all_t<decltype(property)>;
                if constexpr (Property::is_secure) return;
                const auto& value = Property::get_reference(object);
                if constexpr (Property::ValueInfo::is_map_type) {
                    for (const auto& [key, value] : value) {
                        json[property.name()][key] = value;
                    }
                }
                else if constexpr (Property::ValueInfo::is_array_type) {
                    using ArrayValue = typename Property::Value::value_type;
                    json[property.name()] = JSON::array();
                    for (const auto& val : value) {
                        if constexpr (exists<ArrayValue>) {
                            json[property.name()].push_back(to_json(val));
                        }
                        else {
                            json[property.name()].push_back(val);
                        }
                    }
                }
                else if constexpr (Property::ValueInfo::is_base_type) {
                    json[property.name()] = value;
                }
                else {
                    json[property.name()] = to_json(value);
                }
//                else {
//                    static_assert(false);
//                    //Unknown type
//                }
            });
            return json;
        }

    public:

        template <class Class>
        static Class parse_json(const JSON& json) {
            static_assert(exists<Class>);
            Class result = mapper.template create_empty<Class>();
            mapper.template iterate_properties<Class>([&](const auto& property) {
                auto& value = property.get_reference(result);
                _extract(value, property, json);
            });
            return result;
        }

    private:

        template <class Member, class Property>
        static void _extract(Member& member, const Property& property, const JSON& json) {

            if constexpr (check_for_input) {
                check_input(property.name());
            }

            bool exists_in_json = json.find(property.name()) != json.end();

            if (!exists_in_json) {
                if constexpr (Property::is_optional) {
                    return;
                }
                /*else if constexpr (property.is_id) {
                    member = -1;
                    return;
                }*/
                else {
                    throw std::runtime_error(CU_LOG_LOCATION + " Failed to parse JSON for: " + property.to_string());
                }
            }

            JSON json_value = json[property.name()];

            if constexpr (Property::ValueInfo::is_map_type) {
                for (const auto& value : json_value.get<JSON::object_t>()) {
                    member[value.first] = value.second;
                }
            }
            else if constexpr (Property::ValueInfo::is_array_type) {
                using ArrayValue = typename Property::Value::value_type;

                for (const auto& val : json_value) {
                    if constexpr (exists<ArrayValue>) {
                        member.push_back(parse_json<ArrayValue>(val));
                    }
                    else {
                        member.push_back(val.get<ArrayValue>());
                    }
                }
            }
            else if constexpr (Property::ValueInfo::is_custom_type) {
                if constexpr (Property::ValueInfo::is_pointer) {
                    if (json_value.is_null()) {
                        member = nullptr;
                        return;
                    }
                    using Value = typename Property::Value;
                    member = new Value { };
                    *member = parse_json<Value>(json_value);
                }
                else {
                    member = parse_json<Member>(json_value);
                }
            }
            else {
                if constexpr (check_for_input && Property::ValueInfo::is_string) {
                    check_input(json_value.get<std::string>());
                }
                member = json_value.get<Member>();
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

        template <class Class>
        static constexpr bool exists = mapper.exists<Class>;

        template <class Class>
        static void print(const Class& object) {
            static_assert(exists<Class>);
            std::cout << to_json_string(object) << std::endl;
        }

    };

    template <class T> constexpr bool is_json_mapper_v = std::is_base_of_v<is_json_mapper_cheker_base, cu::remove_all_t<T>>;

    inline constexpr JSONMapper<empty_mapper> empty_json_mapper;

}

#endif
