
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


int main() {

    mapping::test();

    Log(mapper.to_string());


    return 0;
}
