
#include "Mapper.hpp"
#include "JsonMapper.hpp"
#include "TestMappingModel.hpp"

#include "mapping_test.hpp"

using namespace mapping;

MAKE_CLASS_INFO(TestMember,
    MAKE_PROPERTY(TestMember, c),
    MAKE_PROPERTY(TestMember, d)
);

MAKE_CLASS_INFO(TestClass,
    MAKE_PROPERTY(TestClass, a),
    MAKE_PROPERTY(TestClass, b),
    MAKE_PROPERTY(TestClass, lett),
    MAKE_PROPERTY(TestClass, enum_value),
    MAKE_PROPERTY(TestClass, enum_class_value),
    MAKE_PROPERTY(TestClass, int_vector),
//    MAKE_PROPERTY(TestClass, string_string_map),
 //   MAKE_PROPERTY(TestClass, string_int_map),
 //   MAKE_PROPERTY(TestClass, int_int_map),
    MAKE_PROPERTY(TestClass, member),
    //MAKE_PROPERTYTestClass, member_pointer),
  //  MAKE_PROPERTY(TestClass, null_member_pointer),
    MAKE_PROPERTY(TestClass, members)//,
  //  MAKE_PROPERTY(TestClass, members_pointers)
);

MAKE_MAPPER(mapper,
            InfoOfTestMember,
            InfoOfTestClass
);

#ifdef USING_JSON
constexpr auto json_mapper = mapping::JSONMapper<mapper>();
#endif

TestClass cl;


void mapping::test() {


    auto getter = &TestClass::get_private_int;
    auto setter = &TestClass::set_private_int;

    Log << (cl.*getter)();
    (cl.*setter)(444343);
    Log << (cl.*getter)();

    cl.a = 10;
    cl.b = 20;
    cl.lett = 'A';

    cl.enum_value = TestEnum::b;
    cl.enum_class_value = TestEnumClass::d;

    cl.member_pointer = new TestMember();
    cl.member_pointer->c = 30;
    cl.member_pointer->d = 40;

    cl.int_vector = { 44, 55, 66 };

    cl.members.push_back({ 1, 2 });
    cl.members.push_back({ 3, 4 });

    cl.members_pointers.push_back(new TestMember { 5, 6 });
    cl.members_pointers.push_back(new TestMember { 7, 8 });

    Log << "Class name" << mapper.class_name<TestClass>();
    Logvar(mapper);

    Log << "Property name" << mapper.property<&TestClass::a>().name();

    Log << mapper.property<&TestClass::int_vector>();
    Log << mapper.property<&TestClass::members>();


    Log << mapper;

#ifdef USING_JSON
    json_mapper.test(cl);
    json_mapper.test(cl);
#endif

};
