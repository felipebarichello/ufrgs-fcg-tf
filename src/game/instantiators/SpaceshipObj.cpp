#include "SpaceshipObj.hpp"

using namespace engine;

namespace game::instantiators {
    VObjectConfig SpaceshipObj() {
        return VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 0.0f, 0.0f))
                .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
            )
            .component(new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg")));
    }
} // namespace game::instantiators