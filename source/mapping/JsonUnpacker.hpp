//
//  JsonUnpacker.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 22/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

#include "JsonMapper.hpp"

namespace mapping {

    template <class RootClass, auto& json_mapper>
    class JsonUnpacker {

        using JSONMapper = std::remove_reference_t<decltype(json_mapper)>;
        using Mapper = typename JSONMapper::Mapper;
        using JSON = typename JSONMapper::JSON;

        static_assert(is_json_mapper_v<JSONMapper>);

        static constexpr auto mapper = JSONMapper::mapper;

        static constexpr bool is_valid = Mapper::template exists<RootClass>();

    public:

        template <class Class>
        static JSON pack_to_json(const Class& object) {
            static_assert(exists<Class>);
            auto json = JSONMapper::template to_json(object);
            mapper.template get_class_info<Class>([&](const auto& class_info) {
                json["json_unpack_id"] = class_info.id;
            });
            return json;
        }

        template <class Class>
        static JSON pack_to_json_string(const Class& object) {
            static_assert(exists<Class>);
            return pack_to_json<Class>(object).dump();
        }

    private:

        template <class Class>
        static constexpr bool exists() {
            return Mapper::template exists<Class>();
        }



    };

}
