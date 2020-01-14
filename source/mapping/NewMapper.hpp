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

namespace mapping {

    template <class ClassesInfo>
    class Mapper {

        static_assert(cu::is_tuple<ClassesInfo>::value);

    public:

        const ClassesInfo classes_info;

        constexpr Mapper(const ClassesInfo info) : classes_info(info) {
            static_assert(_tuple_is_valid(info));
        }

        std::string to_string() const {
            std::string result;
            cu::iterate_tuple(classes_info, [&](auto info) {
                result += info.to_string() + "\n";
            });
            return result;
        }

    private:

        //MARK: - Tuple Check

        template <class ClassInfo>
        static constexpr void _check(bool& value, const ClassInfo&) {
            if constexpr (!is_class_info<ClassInfo>::value) {
                value = false;
            }
        }

        template <class T>
        static constexpr bool _tuple_is_valid(const T& tuple) {
            bool result = true;
            std::apply([&](auto&&... args) {((
                    _check(result, args)
            ), ...);}, tuple);
            return result;
        }

    };

}
