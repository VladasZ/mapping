
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

    int age    = -1;
    int height = -1;

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


    constexpr Cat cat { };

    Cat mutable_cat;

    mapper.get(mutable_cat, &Cat::age) = 111;
    Log(mapper.get(mutable_cat, &Cat::age));

    static_assert(mapper.get(cat, &Cat::age) == -1);

    Logvar(mapper.to_string());

    Logvar(mapper.exists<Cat>());




    return 0;
}
