
#include "mapping_test.hpp"

#include "Log.hpp"
#include "MetaHelpers.hpp"

using namespace cu;
using namespace mapping;


struct Nested {

};

struct Obj {
    Nested a;
    Nested get_value_a() { return a; }
    Nested& get_ref_a() { return a; }
    const Nested& get_const_ref_a() { return a; }
};

using Get = decltype(&Obj::get_value_a);
using GetRef = decltype(&Obj::get_ref_a);
using GetConstRef = decltype(&Obj::get_const_ref_a);

using GetInfo = cu::pointer_to_member_info<Get>;
using GetRefInfo = cu::pointer_to_member_info<GetRef>;
using GetConstRefInfo = cu::pointer_to_member_info<GetConstRef>;


template <class T>
void test_template(const T& obj) {
    using ObjT = decltype(obj);
    Logvar(std::is_pointer_v<T>);
    Logvar(std::is_reference_v<T>);
    Logvar(std::is_pointer_v<ObjT>);
    Logvar(std::is_reference_v<ObjT>);
}


int main() {



    Obj obj;

//    test_template(new Obj());
//    Separator;
//    test_template(Obj());

    Separator;

    Logvar(GetInfo::to_string());
    Logvar(GetRefInfo::to_string());
    Logvar(GetConstRefInfo::to_string());

//    REVEAL_TEMPLATE(Spok);

    Separator;


    Logvar(CU_CLASS_NAME(
            decltype(std::declval<Obj>().get_const_ref_a())
            ));

//    test();

    return 0;
}
