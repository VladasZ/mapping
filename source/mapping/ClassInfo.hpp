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

    template <class _Class, auto& properties, unsigned _id>
    class ClassInfo final : is_class_info_cheker_base {

    private:

        using Properties = std::remove_reference_t<decltype(properties)>;
        static_assert(cu::is_tuple_v<Properties>);

    public:

        using Class = _Class;

        static constexpr auto id = _id;

        const std::string_view name;

        static constexpr bool has_custom_properties = [] {
            bool result = false;
            cu::iterate_tuple(properties, [&](const auto& val) {
                using Property = std::remove_reference_t<decltype(val)>;
                if constexpr (Property::Info::is_custom_type) {
                    result = true;
                }
            });
            return result;
        };

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(tuple_is_valid);
        }

    public:

        template <class Action>
        constexpr static void iterate_properties(const Action& action) {
            cu::iterate_tuple(properties, [&](const auto& property) {
                action(property);
            });
        }

        template <class Pointer, class Action>
        constexpr static void get_property(const Pointer& pointer_to_member, const Action& action) {
            static_assert(cu::is_pointer_to_member_v<Pointer>);
            using PointerInfo = cu::pointer_to_member_info<Pointer>;
            static_assert(cu::is_same_v<typename PointerInfo::Class, Class>);
            iterate_properties([&](const auto& property) {
                using Property = cu::remove_all_t<decltype(property)>;
                if constexpr (cu::is_same_v<Pointer, typename Property::Pointer>) {
                    if (pointer_to_member == property.pointer_to_member) {
                        action(property);
                    }
                }
            });
        }

    private:

        //MARK: - Tuple Check

        static constexpr bool tuple_is_valid = [] {
            cu::iterate_tuple(properties, [&](const auto& val) {
                using Property = std::remove_reference_t<decltype(val)>;
                static_assert(is_property_v<Property>);
                constexpr bool is_same = std::is_same_v<Class, typename Property::Class>;
                constexpr bool is_base = std::is_base_of_v<typename Property::Class, Class>;
                static_assert(is_same || is_base);
            });
            return true;
        };

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

#define MAKE_CLASS_INFO(name, ...)\
constexpr auto properties_of_##name = std::make_tuple(__VA_ARGS__);\
constexpr auto InfoOf##name = mapping::ClassInfo<name, properties_of_##name, __LINE__>(#name)
