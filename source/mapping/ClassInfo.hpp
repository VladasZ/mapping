//
//  ClassInfo.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 24/12/2019.
//  Copyright © 2019 VladasZ. All rights reserved.
//

#pragma once

#include <iostream>

#include "Property.hpp"
#include "MetaHelpers.hpp"

namespace mapping {

    class is_class_info_cheker_base { };

    template <auto& _properties>
    class ClassInfo final : is_class_info_cheker_base {

    private:

        template <class T>
        static constexpr auto _has_custom_type_properties(const T& tuple) {
            bool result = false;
            cu::iterate_tuple(tuple, [&](const auto& val) {
                using Property = std::remove_reference_t<decltype(val)>;
                if constexpr (Property::is_custom_type) {
                    result = true;
                }
            });
            return result;
        }

    private:

        using Properties = cu::remove_all_t<decltype(_properties)>;

        using _FirstPropertyType = cu::first_tuple_type<Properties>;

        static_assert(cu::is_tuple_v<Properties>);
        static_assert(is_property_v<_FirstPropertyType>);

    public:

        using Class = typename _FirstPropertyType::Class;

        const std::string_view name;

        static constexpr auto properties = _properties;
        static constexpr bool has_custom_properties = _has_custom_type_properties(_properties);

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(_tuple_is_valid(properties));
        }

    public:

        template <class Action>
        constexpr static void iterate_properties(const Action& action) {
            cu::iterate_tuple(properties, [&](const auto& property) {
                action(property);
            });
        }

    private:

        //MARK: - Tuple Check

        template <class T>
        static constexpr auto _tuple_is_valid(const T& tuple) {
            bool result = false;
            cu::iterate_tuple(tuple, [&](const auto& val) {
                using Property = std::remove_reference_t<decltype(val)>;
                if constexpr (is_property_v<Property>) {
                    result = true;
                }
                static_assert(std::is_same_v<typename _FirstPropertyType::Class, typename Property::Class>);
            });
            return result;
        }

    public:

        std::string to_string() const {
            std::string result = std::string(name) + "\n";
            result += std::string() + "has custom props: " + (has_custom_properties ? "true" : "false") + "\n";
            iterate_properties([&](const auto& prop){
                result += prop.to_string() + "\n";
            });
            return result;
        }

    };

    template <class T> constexpr bool is_class_info_v = std::is_base_of_v<is_class_info_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_CLASS_INFO(name, properties)\
static constexpr auto properties_of_##name = properties;\
static constexpr auto InfoOf##name = mapping::ClassInfo<properties_of_##name>(#name)
