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

    template <class ClassesInfo, ClassesInfo& classes_info>
    class Mapper {

        static_assert(cu::is_tuple<cu::remove_all_t<ClassesInfo>>::value);

    public:

        constexpr explicit Mapper() {
            static_assert(_tuple_is_valid(classes_info));
        }

    public:

        template <class T>
        constexpr bool exists() const {
            bool result = false;
            cu::iterate_tuple(classes_info, [&](const auto& val) {
                using Info = cu::remove_all_t<decltype(val)>;
                if constexpr (std::is_same_v<T, typename Info::Class>) {
                    result = true;
                }
            });
            return result;
        }

        template <class T>
        constexpr bool get() const {
            static_assert(exists<T>());
            return false;
        }

    private:

        //MARK: - Tuple Check

        template <class T>
        static constexpr bool _tuple_is_valid(const T& tuple) {
            bool result = true;
            cu::iterate_tuple(tuple, [&](const auto& val) {
                if constexpr (!is_class_info_t<decltype(val)>) {
                    result = false;
                }
            });
            return result;
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

}

#define MAKE_MAPPER(...)\
constexpr auto mapper_classes = std::make_tuple(__VA_ARGS__);\
constexpr auto mapper = mapping::Mapper<decltype(mapper_classes), mapper_classes>();
