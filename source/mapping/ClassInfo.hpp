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

    template <class T, auto& properties>
    class ClassInfo final : is_class_info_cheker_base {

    private:

        using Properties = std::remove_reference_t<decltype(properties)>;
        static_assert(cu::is_tuple_v<Properties>);

    private:

        template <auto pointer, class Pointer = decltype(pointer)>
        constexpr static int _property_index() {
            using PointerInfo = cu::pointer_to_member_info<Pointer>;
            static_assert(std::is_same_v<typename PointerInfo::Class, Class>);
            int result = 0;
            cu::indexed_iterate_tuple(properties, [&](auto index, auto property) {
                using Property = decltype(property);
                if constexpr (cu::is_same_v<Pointer, typename Property::Pointer>) {
                    if constexpr (pointer == Property::pointer_to_member) {
                        result = index;
                    }
                }
            });
            return result;
        }

    public:

        using Class = T;
        using This = ClassInfo<T, properties>;

        const std::string_view name;

        static constexpr bool has_custom_properties = [] {
            bool result = false;
            cu::iterate_tuple(properties, [&](auto val) {
                if constexpr (decltype(val)::ValueInfo::is_custom_type) {
                    result = true;
                }
            });
            return result;
        }();

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(tuple_is_valid);
        }

    public:

        template <class Action>
        constexpr static void iterate_properties(Action action) {
            cu::iterate_tuple(properties, action);
        }

        template <auto pointer>
        constexpr static auto property() {
            return std::get<_property_index<pointer>()>(properties);
        }

        template <auto pointer>
        constexpr static auto get_value(Class& obj) {
            return property<pointer>().get_reference(obj);
        }

        static constexpr bool has_id = [] {
            bool result = false;
            iterate_properties([&](auto p) { if (p.is_id) result = true; });
            return result;
        }();

    private:

        //MARK: - Tuple Check

        static constexpr bool tuple_is_valid = [] {
            iterate_properties([&](auto property) {
                using Property = decltype(property);
                static_assert(is_property_v<Property>);
                static_assert(cu::is_related_v<Class, typename Property::Class>);
            });
            return true;
        }();

    public:

        std::string to_string() const {
            std::string result = std::string(name) + "\n";
            result += std::string() + "has custom props: " + (has_custom_properties ? "true" : "false") + "\n";
            iterate_properties([&](auto prop) {
                result += prop.to_string() + "\n";
            });
            return result;
        }

    };

    template <class T> constexpr bool is_class_info_v = std::is_base_of_v<is_class_info_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_CLASS_INFO(name, ...)\
inline constexpr auto properties_of_##name = std::make_tuple(__VA_ARGS__);\
inline constexpr auto InfoOf##name = mapping::ClassInfo<name, properties_of_##name>(#name)
