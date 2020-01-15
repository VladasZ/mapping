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
#include "MetaHelpers.hpp"

namespace mapping {

    enum class PropertyType {
        None,
        Secure,
        Unique
    };

    class is_property_cheker_base { };

    template<auto _pointer, PropertyType type = PropertyType::None>
    class Property : is_property_cheker_base {

        using Pointer = decltype(_pointer);

        static_assert(cu::is_pointer_to_member_v<Pointer>);

        using PointerInfo = cu::pointer_to_member_info<Pointer>;

        const std::string_view _name;

    public:

        using Class = typename PointerInfo::Class;
        using Value = typename PointerInfo::Value;

        static constexpr auto pointer = _pointer;

        static constexpr bool is_string    = std::is_same_v          <Value, std::string>;
        static constexpr bool is_float     = std::is_floating_point_v<Value>;
        static constexpr bool is_integer   = std::numeric_limits     <Value>::is_integer;

        static constexpr bool is_vector = cu::is_vector_v<Value>;

        static constexpr bool is_base_type   = is_string || is_float || is_integer || is_vector;
        static constexpr bool is_array_type  = is_vector;
        static constexpr bool is_custom_type = !is_base_type;

        static constexpr bool is_secure = type == PropertyType::Secure;
        static constexpr bool is_unique = type == PropertyType::Unique;

        static inline const auto class_name = cu::class_name<Class>;

        constexpr Property(const std::string_view& name) : _name(name) {
        }

        std::string name() const {
            return std::string(_name);
        }

        static std::string database_type_name() {
            if constexpr (is_string) {
                return "TEXT";
            }
            else if constexpr (is_float) {
                return "REAL";
            }
            else if constexpr (is_integer) {
                return "INTEGER";
            }
            else {
                Fatal(std::string() + "Invalid member type: " + cu::class_name<Value>);
            }
        }

        std::string to_string() const {
            return std::string() +
                   "Property: " + name() + " type: " + class_name;
        }

    };

    template <class T> constexpr bool is_property_v = std::is_base_of_v<is_property_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_PROPERTY(name, pointer) mapping::Property<pointer>(name)
