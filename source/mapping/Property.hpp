//
//  Property.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 8/28/2019.
//  Copyright © 2019 VladasZ. All rights reserved.
//

#pragma once

#include <string>

#include "Log.hpp"
#include "TypeInfo.hpp"

namespace mapping {

    enum class PropertyType {
        None,
        Secure,
        Unique
    };

    class is_property_cheker_base { };

    template<auto _pointer_to_member, PropertyType type = PropertyType::None>
    class Property : is_property_cheker_base {


        const std::string_view _name;

    public:

        using Pointer = decltype(_pointer_to_member);

        static_assert(cu::is_pointer_to_member_v<Pointer>);

        using PointerInfo = cu::pointer_to_member_info<Pointer>;

        using Class =                       typename PointerInfo::Class;
        using Value = std::remove_pointer_t<typename PointerInfo::Value>;

        static constexpr auto pointer_to_member = _pointer_to_member;

        using Info = cu::TypeInfo<typename PointerInfo::Value>;

        static constexpr bool is_secure = type == PropertyType::Secure;
        static constexpr bool is_unique = type == PropertyType::Unique;

        static constexpr bool is_array_of_custom_types = [] {
            if constexpr (Info::is_array_type) {

                return true;
            }
            else {
                return false;
            }
        }();

        static constexpr bool is_valid = [] {
            if constexpr (Info::is_pointer) {
                static_assert(Info::is_custom_type);
                return Info::is_custom_type;
            }
            else {
                return true;
            }
        }();

        static_assert(is_valid);

        static inline const auto class_name = cu::class_name<Class>;

        constexpr Property(const std::string_view& name) : _name(name) {
        }

        template <class T>
        static constexpr auto& get_value(T& object) {
            static_assert(cu::is_same_v<T, Class>);
            if constexpr (Info::is_pointer) {
                return *(object.*pointer_to_member);
            }
            else {
                return object.*pointer_to_member;
            }
        }

        template <class T>
        static constexpr auto& get_reference(T& object) {
            static_assert(cu::is_same_v<T, Class>);
            return object.*pointer_to_member;
        }

        std::string name() const {
            return std::string(_name);
        }

        static std::string database_type_name() {
            if constexpr (Info::is_string) {
                return "TEXT";
            }
            else if constexpr (Info::is_float) {
                return "REAL";
            }
            else if constexpr (Info::is_integer) {
                return "INTEGER";
            }
            else {
                Fatal(std::string() + "Invalid member type: " + cu::class_name<Value>);
            }
        }

        std::string to_string() const {
            return std::string() + "\n" +
                   "Property: " + name() + "\n" +
                   ", type: " + cu::class_name<Value> + "\n" +
                   ", of class: " + class_name + "\n" +
                   ", is pointer: " + (Info::is_pointer ? "true" : "false") + "\n" +
                   ", is array: " + (Info::is_array_type ? "true" : "false") + "\n" +
                   ", is array of custom types: " + (is_array_of_custom_types ? "true" : "false")
                   ;
        }

    };

    template <class T> constexpr bool is_property_v = std::is_base_of_v<is_property_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_PROPERTY(name, pointer) mapping::Property<pointer>(name)
