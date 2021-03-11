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

    template <class T, auto& _properties>
    class ClassInfo final : is_class_info_cheker_base {

    private:

        using Properties = std::remove_reference_t<decltype(_properties)>;
        static_assert(cu::is_tuple_v<Properties>);

    private:

        template <auto pointer, class Pointer = decltype(pointer)>
        constexpr static int _property_index() {
            using PointerInfo = cu::pointer_to_member_info<Pointer>;
            static_assert(std::is_same_v<typename PointerInfo::Class, Class>);
            int result = -1;
            cu::indexed_iterate_tuple(_properties, [&](auto index, auto property) {
                using Property = decltype(property);
                if constexpr (cu::is_same_v<Pointer, typename Property::Pointer>) {
                    if constexpr (pointer == Property::getter) {
                        result = index;
                    }
                }
            });
            return result;
        }

        template <const std::string_view& name>
        constexpr static int _property_by_name_index() {
            int result = -1;
            cu::indexed_iterate_tuple(_properties, [&](auto index, auto property) {
                if (name == property._name)
                    result = index;
            });
            return result;
        }

    public:

        using Class = T;
        using This = ClassInfo<T, _properties>;

        const std::string_view name;

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(tuple_is_valid);
        }

    public:

        template <class Action>
        constexpr static void properties(Action action) {
            cu::iterate_tuple(_properties, action);
        }

        template <class Action>
        constexpr static void mappable_properties(Action action) {
            properties([&](auto prop) {
                if constexpr (prop.is_mappable())
                    action(prop);
            });
        }

        template <class Class, class Action>
        constexpr static void properties_of_type(Action action) {
            properties([&](auto prop) {
                using Val = typename decltype(prop)::Value;
                if constexpr (std::is_same_v<Class, Val>)
                    action(prop);
            });
        }

        template <const std::string_view& name>
        constexpr static auto property_by_name() {
            return std::get<_property_by_name_index<name>()>(_properties);
        }

        template <auto pointer>
        constexpr static auto property() {
            return std::get<_property_index<pointer>()>(_properties);
        }

        template <auto pointer>
        constexpr static auto get_value(Class& obj) {
            return property<pointer>().get_reference(obj);
        }

        static constexpr bool has_id = [] {
            bool result = false;
            properties([&](auto p) { if (p.is_id) result = true; });
            return result;
        }();

    private:

        //MARK: - Tuple Check

        static constexpr bool tuple_is_valid = [] {
            properties([&](auto prop) {
                using Property = decltype(prop);
                static_assert(is_property_v<Property>);
                static_assert(cu::is_related_v<Class, typename Property::Class>);
            });
            return true;
        }();

    public:

        static constexpr bool has_custom_property = [] {
            bool result = false;
            properties([&](auto prop) {
                if constexpr (prop.is_mappable())
                    result = true;
            });
            return result;
        }();

        std::string to_string() const {
            std::string result = std::string(name) + "\n";
            result += std::string() + "has custom props: " + (has_custom_property ? "true" : "false") + "\n";
            return result;
        }

    };

    template <class T> constexpr bool is_class_info_v = std::is_base_of_v<is_class_info_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_CLASS_INFO(name, ...)\
inline constexpr auto properties_of_##name = std::make_tuple(__VA_ARGS__);\
inline constexpr auto InfoOf##name = mapping::ClassInfo<name, properties_of_##name>(#name)
