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

    using ID = unsigned;

    enum class PropertyType {
        None,
        ID,
        Secure,
        Unique
    };

    template<auto _pointer_to_member, PropertyType type = PropertyType::None, bool optional = false>
    class Property {

    public:

        using Pointer = decltype(_pointer_to_member);
        using PointerInfo = cu::pointer_to_member_info<Pointer>;

        using Class = typename PointerInfo::Class;
        using Value = std::remove_pointer_t<typename PointerInfo::Value>;

        using ValueInfo = cu::TypeInfo<typename PointerInfo::Value>;


        static inline const auto class_name = cu::class_name<Class>;

        static constexpr auto pointer_to_member = _pointer_to_member;

        static constexpr bool is_id     = type == PropertyType::ID;
        static constexpr bool is_secure = type == PropertyType::Secure;
        static constexpr bool is_unique = type == PropertyType::Unique;

        static constexpr bool is_optional = optional;

        template <class T>
        static constexpr bool is_related_class =
            cu::is_same_v<T, Class> || cu::is_base_of_v<Class, T>;

        static constexpr bool is_valid = [] {
            if constexpr (is_id) {
                static_assert(std::is_same_v<Value, ID>, "ID propery must be of unsigned type.");
            }
            if constexpr (ValueInfo::is_base_type) {
                static_assert(!ValueInfo::is_pointer);
                return true;
            }
            else if constexpr (ValueInfo::is_pointer) {
                static_assert(ValueInfo::is_custom_type);
                return true;
            }
            else if constexpr (ValueInfo::is_array_type) {
                //if constexpr (cu::TypeInfo<typename Value::value_type>::is_embedded_type) {
                static_assert(!std::is_pointer_v<typename Value::value_type>);
                //}
                return true;
            }
            else {
                return true;
            }
        }();

        explicit constexpr Property(const std::string_view& name) : _name(name) { }

        template <class T>
        static constexpr auto& get_value(T& object) {
            static_assert(is_related_class<T>);
            if constexpr (std::is_pointer_v<T>) {
                return get_value(*object);
            }
            else if constexpr (ValueInfo::is_pointer) {
                return *(object.*pointer_to_member);
            }
            else {
                return object.*pointer_to_member;
            }
        }

        template <class T>
        static constexpr auto& get_reference(T& object) {
            static_assert(is_related_class<T>);
            if constexpr (std::is_pointer_v<T>) {
                return get_reference(*object);
            }
            else {
                return object.*pointer_to_member;
            }
        }

        std::string name() const {
            return std::string(_name);
        }

        std::string to_string() const {
            return std::string() + "\n" +
                "Property: " + name() + "\n" +
                "type: " + cu::class_name<Value> +"\n" +
                "of class: " + class_name + "\n";
        }

        static_assert(is_valid);
        static_assert(cu::is_pointer_to_member_v<Pointer>);

        private:

            const std::string_view _name;

    };


    template <class> struct is_property : std::false_type { };
    template <auto p, PropertyType t, bool o> struct is_property<Property<p, t, o>> : std::true_type { };
    template <class T> constexpr bool is_property_v = is_property<cu::remove_all_t<T>>::value;

}

#define MAKE_PROPERTY(name, pointer) mapping::Property<pointer>(name)
#define MAKE_ID_PROPERTY(pointer) mapping::Property<pointer, mapping::PropertyType::ID>("id")
#define MAKE_SECURE_PROPERTY(name, pointer) mapping::Property<pointer, mapping::PropertyType::Secure>(name)
