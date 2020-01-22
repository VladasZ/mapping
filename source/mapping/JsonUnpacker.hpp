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

    template <auto& json_mapper>
    class JsonUnpacker {

        using JSONMapper = std::remove_reference_t<decltype(json_mapper)>;
        using Mapper = typename JSONMapper::Mapper;

        static_assert(is_json_mapper_v<JSONMapper>);

        static constexpr auto mapper = Mapper::mapper;

    public:



    };

}
