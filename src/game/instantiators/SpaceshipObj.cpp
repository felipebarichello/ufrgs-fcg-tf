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
            .transform(TransformBuilder()
                .position(Vec3(0.0f, -0.5f, 0.2f))
            )
            .component(out_drawable);
    }

} // namespace game::instantiators