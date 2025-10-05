#pragma once 

#include "linalg.hpp"

namespace engine {
    Mat4 identity_matrix() {
        return Mat4(1.0f);
    }

    Vec3 negate(Vec3 v) {
        return Vec3(-v.x, -v.y, -v.z);
    }
}
