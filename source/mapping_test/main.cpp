
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

constexpr auto CatsInfo = mapping::ClassInfo(
        "Cat",
        std::tuple(
                mapping::Property("age",    &Cat::age   ),
                mapping::Property("height", &Cat::height)
        ));

constexpr auto DogsInfo = mapping::ClassInfo(
        "Dog",
        std::tuple(
                mapping::Property("height", &Dog::height)
        ));

constexpr auto mapper = mapping::Mapper(std::make_tuple(CatsInfo, DogsInfo));


//constexpr auto has_cats = mapper.exists<Cat>();

int main() {


    Logvar(mapper.to_string());

    Logvar(mapper.exists<Cat>());


    return 0;
}
