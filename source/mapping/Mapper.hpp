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

    template <auto& classes_info>
    class Mapper : is_mapper_cheker_base {

        using ClassesInfo = std::remove_reference_t<decltype(classes_info)>;
        static_assert(cu::is_tuple_v<ClassesInfo>);

    public:

        constexpr explicit Mapper() {
            static_assert(_classes_info_is_valid);
        }

    public:

        template <class T, class Action>
        static constexpr void iterate_properties(const Action& action) {
            static_assert(exists<T>());
            using Class = std::remove_pointer_t<T>;
            iterate_classes([&](const auto& info) {
                using ClassInfo = cu::remove_all_t<decltype(info)>;
                constexpr bool is_same = cu::is_same_v<Class, typename ClassInfo::Class>;
                constexpr bool is_base = cu::is_base_of_v<typename ClassInfo::Class, Class>;
                if constexpr (is_same || is_base) {
                    ClassInfo::iterate_properties([&](const auto& property) {
                        action(property);
                    });
                }
            });
        }

        template <class Action>
        static constexpr void iterate_classes(const Action& action) {
            cu::iterate_tuple(classes_info, [&](const auto& info) {
                action(info);
            });
        }

        template <class T>
        static constexpr bool exists() {
            bool result = false;
            using Class = std::remove_pointer_t<T>;
            iterate_classes([&](const auto& val) {
                using Info = cu::remove_all_t<decltype(val)>;
                if constexpr (cu::is_same_v<Class, typename Info::Class>) {
                    result = true;
                }
            });
            return result;
        }

        template <class T, class Action>
        static constexpr void get_class_info(const Action& action) {
            static_assert(exists<T>());
            using Class = std::remove_pointer_t<T>;
            iterate_classes([&] (const auto& info) {
                using Info = cu::remove_all_t<decltype(info)>;
                if constexpr (cu::is_same_v<Class, typename Info::Class>) {
                    action(info);
                }
            });
        }

        template <auto pointer, class Pointer = decltype(pointer), class Action>
        static constexpr void get_property(const Action& action) {
            using Class = typename cu::pointer_to_member_class<Pointer>::type;
            static_assert(exists<Class>());
            get_class_info<Class>([&](const auto& class_info) {
                class_info.template get_property<pointer>([&](const auto& property) {
                    action(property);
                });
            });
        }

        template <auto pointer>
        static std::string get_property_name() {
            std::string result;
            get_property<pointer>([&](auto property) {
               result = property.name();
            });
            return result;
        }

    public:

        template <class Class>
        static constexpr Class create_empty() {
            static_assert(exists<Class>());
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
            static_assert(exists<Class>());
            auto pointer = new Class { };
            *pointer = _create_empty<Class>();
            return pointer;
        }

        template <class Class>
        static constexpr Class _create_empty() {
            static_assert(exists<Class>());
            Class result;
            iterate_properties<Class>([&](const auto& property) {
                using Property = cu::remove_all_t<decltype(property)>;
                using Value = typename Property::Value;
                auto& reference = property.get_reference(result);
                constexpr bool is_custom_type = Property::Info::is_custom_type;
                if constexpr (Property::Info::is_pointer) {
                    static_assert(is_custom_type);
                    reference = nullptr;
                }
                else {
                    if constexpr (is_custom_type) {
                        reference = _create_empty<Value>();
                    }
                    else {
                        reference = Value { };
                    }
                }
            });
            return result;
        }

    public:

        template <class Class>
        static constexpr bool has_id = []{
            bool result = false;
            get_class_info<Class>([&](auto class_info) { result = class_info.has_id; });
            return result;
        }();

        template <class Class>
        static constexpr std::string_view get_class_name() {
            std::string_view result;
            get_class_info<Class>([&](const auto& info) {
                result = info.name;
            });
            return result;
        }

        template <class Class>
        static constexpr bool is_equals(const Class& a, const Class& b) {
            static_assert(exists<Class>());
            bool result = true;
            iterate_properties<Class>([&](const auto& property) {
                if (a.*property.pointer != b.*property.pointer) {
                    result = false;
                }
            });
            return result;
        }

    private:

        //MARK: - Tuple Check

        static constexpr bool _classes_info_is_valid = [] {
            iterate_classes([&](const auto& class_info) {
                using ClassInfo = cu::remove_all_t<decltype(class_info)>;
                static_assert(is_class_info_v<ClassInfo>);
                using Class = typename ClassInfo::Class;
                if constexpr (ClassInfo::has_custom_properties) {
                    ClassInfo::iterate_properties([&](const auto& property) {
                        using Property = cu::remove_all_t<decltype(property)>;
                        if constexpr (Property::Info::is_custom_type) {
                            using Value = typename Property::Value;
                            static_assert(exists<Value>());
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

}

#define MAKE_MAPPER(name, ...)\
static constexpr auto mapper_##name_classes = std::make_tuple(__VA_ARGS__);\
static constexpr auto name = mapping::Mapper<mapper_##name_classes>();
