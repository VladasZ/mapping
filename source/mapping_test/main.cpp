
#include "Log.hpp"
#include "Rect.hpp"
#include "Mapper.hpp"
#include "JsonMapper.hpp"
#include "ArrayUtils.hpp"
#include "TestMappingModels.hpp"

#include "mapping_test.hpp"

class Viha {
public:

    std::vector<int> numbers;
    std::vector<gm::Point> points;

};

MAKE_CLASS_INFO(Viha, std::tuple(
        MAKE_PROPERTY("numbers", &Viha::numbers),
        MAKE_PROPERTY("points", &Viha::points)
));

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
        gm::InfoOfRect,
        InfoOfViha
        );

constexpr auto json_mapper = mapping::JSONMapper<mapper>();


int main() {

    Viha vi;

    vi.numbers = { 1, 2, 3, 4 };
    vi.points = { { 1, 2 }, { 3, 4 } };

    auto json_string = json_mapper.to_json(vi);

    Logvar(json_string);

    auto parsed = json_mapper.parse<Viha>(json_string);

    Logvar(json_mapper.to_json(parsed));

    return 0;
}
