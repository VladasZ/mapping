
#include "Log.hpp"
#include "NewMapper.hpp"
#include "TestJsonModels.hpp"

class Dog {
public:

    int height;

    static constexpr std::string_view class_name() {
        return "Dog";
    }
};

class Cat {
public:

    int age;
    int height;

};

MAKE_CLASS_INFO(Cat, std::tuple(
        mapping::Property("age",    &Cat::age   ),
        mapping::Property("height", &Cat::height)
));

MAKE_CLASS_INFO(Dog, std::tuple(
        mapping::Property("height", &Dog::height)
));

MAKE_MAPPER(InfoOfCat, InfoOfDog);


constexpr auto has_cats = mapper.exists<Cat>();

int main() {


    Logvar(mapper.to_string());

    Logvar(mapper.exists<Cat>());


    return 0;
}
