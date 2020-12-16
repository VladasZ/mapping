//
//  NewMapper.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 24/12/2019.
//  Copyright © 2019 VladasZ. All rights reserved.
//

#pragma once

#include <iostream>

#include "Property.hpp"
#include "ClassInfo.hpp"
#include "ArrayUtils.hpp"

namespace mapping {

    class is_mapper_cheker_base { };

    template <auto& classes>
    class Mapper : is_mapper_cheker_base {

        using This = Mapper<classes>;

        using ClassesInfo = std::remove_reference_t<decltype(classes)>;
        static_assert(cu::is_tuple_v<ClassesInfo>);

    private:

        template <class T>
        static constexpr int _class_index() {
            static_assert(exists<T>);
            int result = -1;
            cu::indexed_iterate_tuple(classes, [&](auto index, auto info) {
                if constexpr (cu::is_same_v<T, typename decltype(info)::Class>) {
                    result = index;
                }
            });
            return result;
        };

        template <class Class>
        static constexpr Class _create_empty() {
            static_assert(exists<Class>);
            Class result;
            iterate_properties<Class>([&](auto property) {
                using Property = decltype(property);
                using Value = typename Property::Value;
                auto& reference = property.get_reference(result);
                if constexpr (Property::ValueInfo::is_pointer) {
                    static_assert(Property::ValueInfo::is_custom_type);
                    reference = nullptr;
                }
                else {
                    if constexpr (Property::ValueInfo::is_custom_type) {
                        //fix
                       // reference = This::template _create_empty<Value>();
                    }
                    else {
                        reference = Value { };
                    }
                }
            });
            return result;
        }

    public:

        constexpr explicit Mapper() {
            static_assert(_classes_info_is_valid);
        }

        template <class T, class Action>
        static constexpr void iterate_properties(Action action) {
            info<T>().iterate_properties(action);
        }

        template <class Action>
        static constexpr void iterate_classes(Action action) {
            cu::iterate_tuple(classes, action);
        }

        template <class T>
        static constexpr bool exists = [] {
            bool result = false;
            using Class = std::remove_pointer_t<T>;
            iterate_classes([&](auto val) {
                using Info = decltype(val);
                if constexpr (cu::is_same_v<Class, typename Info::Class>) {
                    result = true;
                }
            });
            return result;
        }();

        template <class T>
        static constexpr auto info() {
            return std::get<_class_index<T>()>(classes);
        };

        template <auto pointer, class Pointer = decltype(pointer), class Class = cu::pointer_to_member_class_t<Pointer>>
        static constexpr auto property() {
            static_assert(exists<Class>);
            return info<Class>().template property<pointer>();
        }

        template <auto pointer>
        static std::string get_property_name() {
            static constexpr auto p = property<pointer>();
            if constexpr (p.is_id) {
               return "rowid";
            }
            else {
               return p.name();
            }
        }

    public:

        template <class Class>
        static constexpr Class create_empty() {
            static_assert(exists<Class>);
            if constexpr (std::is_pointer_v<Class>) {
                return _allocate_empty<cu::remove_all_t<Class>>();
            }
            else {
                return _create_empty<Class>();
            }
        }

    private:

        template <class Class>
        static constexpr Class* _allocate_empty() {
            static_assert(exists<Class>);
            auto pointer = new Class { };
            *pointer = _create_empty<Class>();
            return pointer;
        }

    public:

        template <class Class>
        static constexpr bool has_id = []{
            return info<Class>.has_id;
        }();

        template <class Class>
        static constexpr std::string_view class_name() {
            static_assert(exists<Class>);
            return info<Class>().name;
        }

        template <class Class>
        static constexpr bool is_equals(const Class& a, const Class& b) {
            static_assert(exists<Class>);
            bool result = true;
            iterate_properties<Class>([&](auto property) {
                if (a.*property.pointer != b.*property.pointer) {
                    result = false;
                }
            });
            return result;
        }

    private:

        //MARK: - Tuple Check

        static constexpr bool _classes_info_is_valid = [] {
            iterate_classes([&](auto class_info) {
                using ClassInfo = decltype(class_info);
                static_assert(is_class_info_v<ClassInfo>);
                if constexpr (ClassInfo::has_custom_properties) {
                    ClassInfo::iterate_properties([](auto property) {
                        using Property = decltype(property);
                        if constexpr (Property::ValueInfo::is_custom_type) {
                            using Value = typename Property::Value;
                            static_assert(exists<Value>);
                        }
                    });
                }
            });
            return true;
        }();

    public:

        std::string to_string() const {
            std::string result;
            iterate_classes([&](const auto& info) {
                result += info.to_string() + "\n";
            });
            return result;
        }

    };

    template <class T> constexpr bool is_mapper_v = std::is_base_of_v<is_mapper_cheker_base, cu::remove_all_t<T>>;

    inline constexpr std::tuple<> __empty_tuple;
    inline constexpr mapping::Mapper<__empty_tuple> empty_mapper;

}

#define MAKE_MAPPER(name, ...)\
inline constexpr auto mapper_##name_classes = std::make_tuple(__VA_ARGS__);\
inline constexpr auto name = mapping::Mapper<mapper_##name_classes>();
