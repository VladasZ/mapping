
#include "mapping_test.hpp"

#include "Log.hpp"
#include "MetaHelpers.hpp"

using namespace cu;
using namespace mapping;


class Obj {

};

template <class T>
void test_template(const T& obj) {
    using ObjT = decltype(obj);

    Logvar(class_name(obj));
    Logvar(class_name<T>());
    Logvar(class_name<ObjT>());
    Logvar(std::is_pointer_v<T>);
    Logvar(std::is_reference_v<T>);
    Logvar(std::is_pointer_v<ObjT>);
    Logvar(std::is_reference_v<ObjT>);
}


int main() {


    test_template(new Obj());
    Separator;
    test_template(Obj());

//    test();

    return 0;
}
