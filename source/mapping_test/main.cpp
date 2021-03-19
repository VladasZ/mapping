
#include "mapping_test.hpp"

#include "Log.hpp"
#include "TypeInfo.hpp"
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

template <class T>
void test_template(const T& obj) {
    Log << TypeInfo<T>::to_string();
}


int main() {

    Obj obj;

    test_template(new Obj());
    Separator;
    test_template(Obj());

    return 0;
}
