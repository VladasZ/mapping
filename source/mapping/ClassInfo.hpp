//
//  ClassInfo.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 24/12/2019.
//  Copyright © 2019 VladasZ. All rights reserved.
//

#pragma once

#include <iostream>

#include "MetaHelpers.hpp"

namespace mapping {

    class is_class_info_cheker_base { };

    template <class Properties, Properties& properties>
    class ClassInfo final : is_class_info_cheker_base {

        using _FirstPropertyType = cu::first_tuple_type<Properties>;

        static_assert(cu::is_tuple_v<Properties>);
        static_assert(is_property_v<_FirstPropertyType>);

    public:

        static constexpr bool is_class_info = true;

        using Class = typename _FirstPropertyType::Class;

        const std::string_view name;

        constexpr explicit ClassInfo(const std::string_view name) : name(name) {
            static_assert(_tuple_is_valid(properties));
        }

        std::string to_string() const {
            std::string result = std::string(name) + "\n";
            cu::iterate_tuple(properties, [&](const auto& prop){
                result += prop.to_string() + "\n";
            });
            return result;
        }

    private:

        //MARK: - Tuple Check

        template <class T>
        static constexpr auto _tuple_is_valid(const T& tuple) {
            bool result = false;
            cu::iterate_tuple(tuple, [&](const auto& val) {
                using Property = std::remove_reference_t<decltype(val)>;
                if constexpr (is_property_v<Property>) {
                    result = true;
                }
                static_assert(std::is_same_v<typename _FirstPropertyType::Class, typename Property::Class>);
            });
            return result;
        }

    };

    template <class T> constexpr bool is_class_info_t = std::is_base_of_v<is_class_info_cheker_base, cu::remove_all_t<T>>;

}

#define MAKE_CLASS_INFO(name, properties)\
constexpr auto properties_of_##name = properties;\
constexpr auto InfoOf##name = mapping::ClassInfo<decltype(properties_of_##name), properties_of_##name>(#name)
