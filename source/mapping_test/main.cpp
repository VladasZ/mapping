
#include "Log.hpp"
#include "Rect.hpp"
#include "Mapper.hpp"
#include "JsonMapper.hpp"
#include "ArrayUtils.hpp"
#include "TestMappingModels.hpp"

#include "mapping_test.hpp"

namespace gm {

    MAKE_CLASS_INFO(Point, std::tuple(
            MAKE_PROPERTY("x", &Point::x),
            MAKE_PROPERTY("y", &Point::y)
    ));

    MAKE_CLASS_INFO(Size, std::tuple(
            MAKE_PROPERTY("width", &Size::width),
            MAKE_PROPERTY("height", &Size::height)
    ));

    MAKE_CLASS_INFO(Rect, std::tuple(
            MAKE_PROPERTY("origin", &Rect::origin),
            MAKE_PROPERTY("size", &Rect::size)
    ));

}

MAKE_MAPPER(
        gm::InfoOfPoint,
        gm::InfoOfSize,
        gm::InfoOfRect);

constexpr auto json_mapper = mapping::JSONMapper<mapper>();


int main() {

    mapping::test();

    Log(mapper.to_string());


    gm::Rect rect = { 1, 2, 3, 4 };

    auto rect_json = json_mapper.to_json(rect);

    Logvar(rect_json);


    auto parsed_rect = json_mapper.parse<gm::Rect>(rect_json);

    Log(parsed_rect.to_string());

    //Log(json_mapper.to_json(json_mapper.parse<gm::Rect>(json_mapper.to_json(rect))));


    return 0;
}
