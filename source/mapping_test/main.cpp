 
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

constexpr auto pointer = &Spec<float>::a;

template <class T>
constexpr auto ptr = &Spec<T>::a;


constexpr auto ko = ptr<int>;


int main() {

    test();

    // Log << json_mapper.to_json(Spec<int>());

    return 0;
}
