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

    enum TestEnum { a = 11, b = 12 };

    enum class TestEnumClass { c = 10, d = 20 };

    class TestClass {
    public:

        int a;
        int b;
        char lett;

        TestEnum enum_value;
        TestEnumClass enum_class_value;

        std::vector<int> int_vector;

        TestMember member;

        TestMember* member_pointer = new TestMember();
        TestMember* null_member_pointer = nullptr;

        std::vector<TestMember> members;
        std::vector<TestMember*> members_pointers;

        TestClass() { };
        ~TestClass() {
   /*         if (member_pointer != nullptr) {
                delete member_pointer;
            }
            if (null_member_pointer != nullptr) {
                delete member_pointer;
            }
            for (auto mem : members_pointers) {
                delete mem;
            }*/
        }

        std::string to_string() const { return VarString(a) + " " + VarString(b) + " " + VarString(lett) + " " + VarString(member_pointer->to_string());
        }
    };

}
