//
//  JsonMapper.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 15/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

#include <iostream>

#include "json.hpp"

#include "Mapper.hpp"
#include "ExceptionCatch.hpp"

namespace mapping {

    template <auto& mapper>
    class JSONMapper {

        using Mapper = cu::remove_all_t<decltype(mapper)>;

        static_assert(is_mapper_v<Mapper>);

    public:

        template <class Class>
        static void test(const Class& object) {
            auto json = to_json(object);
            Log(json);
            auto parsed_object = parse<Class>(json);
            auto new_json = to_json(parsed_object);
            Log(new_json);
            assert(json == new_json);
        }

        template <class Class>
        static Class parse(const std::string& json_string) {
            static_assert(_exists<Class>());
            return _parse<Class>(nlohmann::json::parse(json_string, nullptr, false));
        }

        template <class Class>
        static std::string to_json(const Class& object) {
            static_assert(_exists<Class>());
            return _to_json(object).dump();
        }

        template<class Array>
        static std::string array_to_json(const Array& array) {
            using Class = typename Array::value_type;
            static_assert(_exists<Class>());
            auto result = nlohmann::json::array();
            for (const auto& value : array) {
                result.push_back(_to_json(value));
            }
            return result.dump();
        }

    private:

        template <class Class>
        static nlohmann::json _to_json(const Class& object) {
            static_assert(_exists<Class>());
            nlohmann::json json;
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                class_info.iterate_properties([&](const auto& property) {
                    using Property = cu::remove_all_t<decltype(property)>;
                    const auto& value = Property::get_value(object);
                    if constexpr (Property::Info::is_array_type) {
                        using ArrayValue = typename Property::Value::value_type;
                        json[property.name()] = nlohmann::json::array();
                        for (const auto& val : value) {
                            if constexpr (_exists<ArrayValue>()) {
                                json[property.name()].push_back(_to_json(val));
                            }
                            else {
                                json[property.name()].push_back(val);
                            }
                        }
                    }
                    else if constexpr (Property::Info::is_base_type) {
                        if constexpr (Property::Info::is_pointer) {
                            if (value == nullptr) {
                                json[property.name()] = nlohmann::json();
                            }
                            else {
                                json[property.name()] = *value;
                            }
                        }
                        else {
                            json[property.name()] = value;
                        }
                    }
                    else {
                        json[property.name()] = _to_json(value);
                    }
                });
            });
            return json;
        }


        template <class Class>
        static Class _parse(const nlohmann::json& json) {
            static_assert(_exists<Class>());
            Class result = mapper.template create_empty<Class>();
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                class_info.iterate_properties([&](const auto& property) {
                    auto& value = property.get_value(result);
                    Logvar(cu::class_name<cu::remove_all_t<decltype(value)>>);
                    Logvar(property.name());
                    _extract(value, property, json);
                });
            });
            return result;
        }

        template <class Member, class Property>
        static void _extract(Member& member, const Property& property, const nlohmann::json& json) {

            nlohmann::json json_value = json[property.name()];

            Log(json_value.dump());
            Log(property.name());

            if constexpr (Property::Info::is_array_type) {
                using ArrayValue = typename Property::Value::value_type;

                Logvar(cu::class_name<ArrayValue>);

                for (const auto& val : json_value) {
                    if constexpr (_exists<ArrayValue>()) {
                        if constexpr (Property::Info::is_pointer) {
                            member.push_back(new ArrayValue(_parse<ArrayValue>(val)));
                        }
                        else {
                            member.push_back(_parse<ArrayValue>(val));
                        }
                    }
                    else {
                        member.push_back(val.get<ArrayValue>());
                    }
                }
            }
            else if constexpr (Property::Info::is_custom_type) {
                member = _parse<Member>(json_value);
            }
            else {
                member = json_value.get<Member>();
            }
        }

        template <class Class>
        static constexpr bool _exists() {
            return mapper.template exists<Class>();
        }

    };

}
