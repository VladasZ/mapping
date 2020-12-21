 
#include "Log.hpp"
#include "JsonMapper.hpp"
#include "mapping_test.hpp"

using namespace mapping;

template <class T>
class Spec {
public:
    T a;
    T b;
};

template <class T>
constexpr auto t_a = mapping::Property<&Spec<T>::a>("a", "Spec");

template <class T>
constexpr auto t_b = mapping::Property<&Spec<T>::a>("b", "Spec");

template <class T>
constexpr auto properties_of_Spec = std::make_tuple(t_a<T>, t_b<T>);



template <class T>
constexpr auto InfoOfSpec = mapping::ClassInfo<Spec<T>, properties_of_Spec<T>>("Spec");


struct InferByCall {

    template <class T>
    static void print(const T& value) {
        Log << value;
    }

};

template <class T>
static void infer_by_call(const T& value) {
    Log << value;
}

int main() {

    Log << InfoOfSpec<float>;

    InferByCall::print(5);

    infer_by_call(5);

    test();

    // Log << json_mapper.to_json(Spec<int>());

    return 0;
}
