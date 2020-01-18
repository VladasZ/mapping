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

        using ClassesInfo = cu::remove_all_t<decltype(classes_info)>;

        static_assert(cu::is_tuple_v<ClassesInfo>);

    public:

        constexpr explicit Mapper() {
            static_assert(_check_classes_info(classes_info));
        }

    public:

        template <class T>
        static constexpr bool exists() {
            bool result = false;
            using Class = std::remove_pointer_t<T>;
            cu::iterate_tuple(classes_info, [&](const auto& val) {
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
            cu::iterate_tuple(classes_info, [&] (const auto& info) {
                using Info = cu::remove_all_t<decltype(info)>;
                if constexpr (cu::is_same_v<Class, typename Info::Class>) {
                    action(info);
                }
            });
        }

        template <class Pointer, class Action>
        static constexpr void get_property(const Pointer& pointer, const Action& action) {
            static_assert(cu::is_pointer_to_member_v<Pointer>);
            using PointerInfo = cu::pointer_to_member_info<Pointer>;
            using Class = typename PointerInfo::Class;
            static_assert(exists<Class>());
            get_class_info<Class>([&](const auto& class_info) {
                class_info.template get_property(pointer, [&](const auto& property) {
                    action(property);
                });
            });

        }

        template <class Class>
        static constexpr Class create_empty() {
            static_assert(exists<Class>());
            Class result;
            get_class_info<Class>([&](const auto& class_info) {
                class_info.iterate_properties([&](const auto& property) {
                    using Property = cu::remove_all_t<decltype(property)>;
                    using Value = typename Property::Value;
                    auto& reference = property.get_reference(result);
               //     Log(property.to_string());
                    if constexpr (Property::Info::is_pointer) {
                        reference = new Value { };
                    }
                    else {
                        reference = Value { };
                    }
                });
            });
            return result;
        }

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
            get_class_info<Class>([&](const auto& class_info) {
                class_info.iterate_properties([&](const auto& property) {
                    if (a.*property.pointer != b.*property.pointer) {
                        result = false;
                    }
                });
            });
            return result;
        }

    private:

        //MARK: - Tuple Check

        template <class T>
        static constexpr bool _check_classes_info(const T& tuple) {
            constexpr auto tuple_size = std::tuple_size_v<T>;

            cu::iterate_tuple(tuple, [&](const auto& class_info) {

                using ClassInfo = cu::remove_all_t<decltype(class_info)>;
                using Class = typename ClassInfo::Class;
                static_assert(is_class_info_v<ClassInfo>);

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
        }

    public:

        std::string to_string() const {
            std::string result;
            cu::iterate_tuple(classes_info, [&](const auto& info) {
                result += info.to_string() + "\n";
            });
            return result;
        }

    };

    template <class T> constexpr bool is_mapper_v = std::is_base_of_v<is_mapper_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_MAPPER(...)\
constexpr auto mapper_classes = std::make_tuple(__VA_ARGS__);\
constexpr auto mapper = mapping::Mapper<mapper_classes>();
