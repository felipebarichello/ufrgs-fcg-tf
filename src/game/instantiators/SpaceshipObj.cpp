#include "SpaceshipObj.hpp"

using namespace engine;

namespace game::instantiators {

    VObjectConfig SpaceshipObj() {
        ObjDrawable* out;
        return SpaceshipObj(out);
    }

    VObjectConfig SpaceshipObj(ObjDrawable*& out_drawable) {
        out_drawable = new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg"));

        return VObjectConfig()
            .component(out_drawable);
    }

} // namespace game::instantiators