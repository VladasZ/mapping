//
//  TestMappingModels.hpp
//  mapping
//
//  Created by Vladas Zakrevskis on 9/01/2020.
//  Copyright © 2020 VladasZ. All rights reserved.
//

#pragma once

#include "Mapper.hpp"

namespace mapping {

    class Private {
    private:
        int a;
    };


    class Simple {
    public:
        int a;
    };

    class TestMember { public:
        int c = 0, d = 0;
        std::string to_string() const { return std::string() + VarString(c) + " " + VarString(d); }
    };

    class TestClass {
    public:

        int a;
        int b;
        char lett;

        std::vector<int> int_vector;

        std::vector<TestMember> members;
        std::vector<TestMember*> members_pointers;

        TestMember* member_pointer = nullptr;

        TestClass() = default;
        ~TestClass() {
            if (member_pointer != nullptr) {
                delete member_pointer;
            }
            for (auto mem : members_pointers) {
                delete mem;
            }
        }

        std::string to_string() const { return VarString(a) + " " + VarString(b) + " " + VarString(lett) + " " + VarString(member_pointer->to_string());
        }
    };

}
