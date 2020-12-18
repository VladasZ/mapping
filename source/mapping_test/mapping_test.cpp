
#include "Mapper.hpp"
#include "JsonMapper.hpp"
#include "TestMappingModels.hpp"

#include "mapping_test.hpp"

using namespace mapping;

struct Unrelated {
    int a;
};

struct Macbook {
    int price;
    constexpr Macbook(int price) : price(price) { }
};

MAKE_CLASS_INFO(Macbook,
    MAKE_PROPERTY("price", &Macbook::price)
);

MAKE_CLASS_INFO(TestMember,
                MAKE_PROPERTY("a", &TestMember::c)
, MAKE_PROPERTY("b", &TestMember::d)
);

MAKE_CLASS_INFO(TestClass,
    MAKE_PROPERTY("a", &TestClass::a),
    MAKE_PROPERTY("b", &TestClass::b),
    MAKE_PROPERTY("lett", &TestClass::lett),
    MAKE_PROPERTY("enum", &TestClass::enum_value),
    MAKE_PROPERTY("enum_class", &TestClass::enum_class_value),
    MAKE_PROPERTY("int_vector", &TestClass::int_vector),
    MAKE_PROPERTY("string_string_map", &TestClass::string_string_map),
    MAKE_PROPERTY("string_int_map", &TestClass::string_int_map),
    MAKE_PROPERTY("int_int_map", &TestClass::int_int_map),
    MAKE_PROPERTY("member", &TestClass::member),
    //MAKE_PROPERTY("member_pointer",      &TestClass::member_pointer),
    MAKE_PROPERTY("null_member_pointer", &TestClass::null_member_pointer),
    MAKE_PROPERTY("members", &TestClass::members)//,
  //  MAKE_PROPERTY("members_pointers",    &TestClass::members_pointers)
);

MAKE_MAPPER(mapper,
            InfoOfMacbook,
            InfoOfTestMember,
            InfoOfTestClass
);

#ifdef USING_JSON
constexpr auto json_mapper = mapping::JSONMapper<mapper>();
#endif

TestClass cl;

static constexpr const std::string_view& b_string = "b";


//Limited by the technology of my time
static constexpr const std::string_view& price_string = "price";

static constexpr Macbook makbok = Macbook(999999959);

static_assert(mapper.value_by_name<price_string>(makbok) == 999999959);



void mapping::test() {

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

    Log << "Property name" << mapper.property_name<&TestClass::a>();

    Log << mapper.property<&TestClass::int_vector>();
    Log << mapper.property<&TestClass::members>();



    Log << mapper.property_by_name<TestClass, b_string>();

    Log << mapper.value_by_name<b_string>(cl);


    Log << mapper;

#ifdef USING_JSON
    json_mapper.test(cl);
#endif

};
