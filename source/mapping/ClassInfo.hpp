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

    template <class Properties>
    class ClassInfo {

        using _FirstPropertyType = cu::first_tuple_type<Properties>;

        static_assert(cu::is_tuple_v<Properties>);
        static_assert(is_property_v<_FirstPropertyType>);

    public:

        static constexpr bool is_class_info = true;

        using Class = typename _FirstPropertyType::Class;

        static constexpr std::string_view name;
        const Properties properties;

        const int spes = 12;

        constexpr ClassInfo(const std::string_view name, const Properties props) : properties(props) {
            static_assert(_tuple_is_valid(props));
        }

        std::string to_string() const {
            std::string result = std::string(name) + "\n";
            cu::iterate_tuple(properties, [&](auto prop){
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

    template <class  > struct is_class_info               : std::false_type { };
    template <class T> struct is_class_info<ClassInfo<T>> : std::true_type  { };
    template <class T> constexpr bool is_class_info_t = is_class_info<cu::remove_all_t<T>>::value;

}
