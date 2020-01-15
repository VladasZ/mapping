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
        static std::string to_json(const Class& object) {
            static_assert(mapper.template exists<Class>());
            nlohmann::json json;
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                cu::iterate_tuple(class_info.properties, [&](const auto& property) {
                    json[std::string(property.name)] = object.*property.pointer;
                });
            });
            return json.dump();
        }

        template <class Class>
        static Class parse(const std::string& json_string) {
            auto json = nlohmann::json::parse(json_string, nullptr, false);
            Class result;
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                cu::iterate_tuple(class_info.properties, [&](const auto& property) {
                    using Property = cu::remove_all_t<decltype(property)>;
                    using Value = typename Property::Value;
                    constexpr auto pointer = Property::pointer;
                    auto& value = mapper.get(result, pointer);
                 //   mapper.get(result, pointer) = json.value(std::string(property.name), Value { });
                    _extract(value, property, json);
                });
            });
            return result;
        }

    private:

        template <class Member, class Property>
        static void _extract(Member& member, const Property& property, const nlohmann::json& json) {
#ifdef __cpp_exceptions
            try {
                member = json.value<Member>(std::string(property.name), Member { });
            }
            catch (...) {
                Fatal(std::string() +
                      "Invalid json value for key: \"" + std::string(property.name) + "\" of class: " + std::string(property.class_name) + ". " +
                      "Expected type: " + std::string(property.database_type_name()) + " " +
                      "JSON exception: " + what()
                );
            }
#else
            member = json.value<Value>(std::string(property.name), Value { });
#endif
        }

    };


}
