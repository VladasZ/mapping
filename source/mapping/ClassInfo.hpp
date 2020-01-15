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

    template <class Properties, Properties& properties>
    class ClassInfo final : is_class_info_cheker_base {

        using _FirstPropertyType = cu::first_tuple_type<Properties>;

        static_assert(cu::is_tuple_v<Properties>);
        static_assert(is_property_v<_FirstPropertyType>);

    public:

        using Class = typename _FirstPropertyType::Class;

        const std::string_view name;

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(_tuple_is_valid(properties));
        }

    public:

        template <class Pointer, Pointer pointer, class Action>
        static constexpr void get_property(const Action& action) {
            static_assert(cu::is_pointer_to_member_v<Pointer>);
            using PointerInfo = cu::pointer_to_member_info<Pointer>;
            static_assert(cu::is_same_v<typename PointerInfo::Class, Class>);
            cu::iterate_tuple(properties, [&](const auto& property) {
                using Property = cu::remove_all_t<decltype(property)>;
                if constexpr (pointer == Property::pointer) {
                    action(property);
                }
            });
        }

        template <class Pointer, Pointer pointer>
        static constexpr std::string_view get_property_name() {
            std::string_view result;
            get_property<Pointer, pointer>([&](const auto& property) {
                result = property.name;
            });
            return result;
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
            cu::iterate_tuple(properties, [&](const auto& prop){
                result += prop.to_string() + "\n";
            });
            return result;
        }

    };

    template <class T> constexpr bool is_class_info_t = std::is_base_of_v<is_class_info_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_CLASS_INFO(name, properties)\
constexpr auto properties_of_##name = properties;\
constexpr auto InfoOf##name = mapping::ClassInfo<decltype(properties_of_##name), properties_of_##name>(#name)
