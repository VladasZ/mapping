
#include "Log.hpp"
#include "Mapper.hpp"
#include "TestJsonModels.hpp"

class Dog {
public:

    int age;
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
        MAKE_PROPERTY("age",    &Cat::age   ),
        MAKE_PROPERTY("height", &Cat::height)
));

MAKE_CLASS_INFO(Dog, std::tuple(
        MAKE_PROPERTY("height", &Dog::height)
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

    mapper.get_class_info<Cat>([&](const auto& info) {
        Logvar(info.to_string());
        Log("Speeess");
    });


    Logvar(mapper.get_class_name<Cat>());

    static_assert(mapper.get_class_name<Cat>() == "Cat");

    InfoOfCat.get_property<decltype(&Cat::age), &Cat::age>([&](const auto& property) {
        Logvar(property.name);
    });

   constexpr auto name = InfoOfCat.get_property_name<decltype(&Cat::age), &Cat::age>();


    static_assert(name == "age");

    return 0;
}
