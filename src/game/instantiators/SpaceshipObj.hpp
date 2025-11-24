#pragma once

#include <engine>

namespace game::instantiators {
    engine::VObjectConfig SpaceshipObj();
    engine::VObjectConfig SpaceshipObj(engine::ObjDrawable*& out_drawable);
}
