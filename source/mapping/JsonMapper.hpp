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
        static Class parse(const std::string& json_string) {
            return _parse<Class>(nlohmann::json::parse(json_string, nullptr, false));
        }

        template <class Class>
        static std::string to_json(const Class& object) {
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
                    auto& value = object.*Property::pointer;
                    if constexpr (Property::is_base_type) {
                        json[property.name()] = value;
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
            static Class result;
            result = Class { };
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                class_info.iterate_properties([&](const auto& property) {
                    using Property = cu::remove_all_t<decltype(property)>;
                    using Value = typename Property::Value;
                    static constexpr auto pointer = Property::pointer;
                    static constexpr auto& value = mapper.get(result, pointer);
                    _extract(value, property, json);
                });
            });
            return result;
        }

        template <class Member, class Property>
        static void _extract(Member& member, const Property& property, const nlohmann::json& json) {
            if constexpr (Property::is_custom_type) {
                Logvar(json.dump());
                Logvar(member.to_string());
                auto parsed = _parse<Member>(json);
                Logvar(parsed.to_string());
                member = _parse<Member>(json);
                member = Member { 1, 2 };
                Logvar(member.to_string());
            }
            else {
#ifdef __cpp_exceptions
                try {
                    member = json.value<Member>(std::string(property.name()), Member { });
                }
                catch (...) {
                    Fatal(std::string() +
                          "Invalid json value for key: \"" + property.name() + "\" of class: " + property.class_name + ". " +
                          "Expected type: " + property.database_type_name() + " " +
                          "JSON exception: " + what()
                    );
                }
#else
                member = json.value<Value>(std::string(property.name), Value { });
#endif
            }
        }

        template <class Class>
        static constexpr bool _exists() {
            return mapper.template exists<Class>();
        }

    };

}
