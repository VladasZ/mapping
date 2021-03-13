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

    using ID = int;

    enum class PropertyType {
        None,
        ID,
        Secure,
        Unique
    };

    template<auto _getter,
             auto _setter = int { },
             PropertyType type = PropertyType::None,
             bool optional = false>

    class Property {

    public:

        using Getter = decltype(_getter);
        using Setter = decltype(_setter);

        using Pointer = decltype(_getter);
        using PointerInfo = cu::pointer_to_member_info<Pointer>;

        using Class = typename PointerInfo::Class;
        using Value = std::remove_pointer_t<typename PointerInfo::Value>;

        using ValueInfo = cu::TypeInfo<typename PointerInfo::Value>;

        static constexpr auto getter = _getter;
        static constexpr auto setter = _setter;

        static constexpr bool is_id = type == PropertyType::ID;
        static constexpr bool is_secure = type == PropertyType::Secure;
        static constexpr bool is_unique = type == PropertyType::Unique;

        static constexpr bool is_optional = optional;

        static constexpr bool is_container = ValueInfo::is_std_vector;

        static constexpr bool get_set = !cu::is_int<Setter>;

        template <class T>
        static constexpr bool is_related_class =
                cu::is_same_v<T, Class> || cu::is_base_of_v<Class, T>;

        static constexpr bool is_valid = [] {
            if constexpr (is_id) {
                static_assert(std::is_same_v<Value, ID>, "ID propery must be of int type.");
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

        static constexpr bool is_mappable() {
            if constexpr (ValueInfo::is_custom_type) {
                return true;
            }
            else if constexpr (ValueInfo::is_std_vector) {
                using ArrayValue = typename ValueInfo::Class::value_type;
                using ArrayValueInfo = cu::TypeInfo<ArrayValue>;
                return ArrayValueInfo::is_custom_type;
            }
            else {
                return false;
            }
        }

        explicit constexpr Property(const std::string_view& name, const std::string_view& class_name) : _name(name), _class_name(class_name) { }

        static constexpr const Value get_value(const Class& object) {
            if constexpr (get_set) {
                if constexpr (ValueInfo::is_pointer) {
                    return *(object.*getter)();
                }
                else {
                    return (object.*getter)();
                }
            }
            else {
                return object.*getter;
            }
        }

        static constexpr void set_value(Class& object, const Value& value) {
            if constexpr (get_set) {
                return (object.*setter)(value);
            }
            else {
                object.*getter = value;
            }
        }

        std::string name() const {
            return std::string(_name);
        }

        std::string class_name() const {
            return std::string(_class_name);
        }

        std::string to_string() const {
            return std::string() + "\n" +
                   "Property: " + name() + "\n" +
                   "type: " + cu::class_name<Value>() +"\n" +
                   "of class: " + class_name() + "\n";
        }

        std::string foreign_key() const {
            return std::string() +
                   class_name() + "_" + name() + "_id";
        }

        static_assert(is_valid);
        static_assert(std::is_member_pointer_v<Pointer>);

    public:

        const std::string_view _name;
        const std::string_view _class_name;

    };


    template <class> struct is_property : std::false_type { };
    template <auto g, auto s, PropertyType t, bool o> struct is_property<Property<g, s, t, o>> : std::true_type { };
    template <class T> constexpr bool is_property_v = is_property<cu::remove_all_t<T>>::value;

}

#define MAKE_PROPERTY(type, name) mapping::Property<&type::name>(#name, #type)
#define MAKE_ID_PROPERTY(type, name) mapping::Property<&type::name, mapping::PropertyType::ID>("id", #type)
#define MAKE_SECURE_PROPERTY(type, name) mapping::Property<&type::name, mapping::PropertyType::Secure>(#name, #type)

#define MAKE_GETTER_PROPERTY(type, name) \
mapping::Property<&type::name,           \
                  &type::set_##name      \
>(#name, #type)

//
//PropertyType type = PropertyType::None,
//bool optional = false,
//auto getter = int { },
//auto setter = int { }