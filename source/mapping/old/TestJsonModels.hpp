//
//  TestJsonModels.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 9/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

namespace mapping {

    class TestClass {

    public:

        uint8_t age = 100;
        uint8_t page = 110;
        uint8_t wage = 120;
        uint8_t gage = 130;

    };

    MAKE_CLASS_INFO(TestClass,
                    std::tuple(
                            MAKE_PROPERTY("age",    &TestClass::age),
                            MAKE_PROPERTY("page", &TestClass::page),
                            MAKE_PROPERTY("wage", &TestClass::wage),
                            MAKE_PROPERTY("gage", &TestClass::gage)
                    ));

}
