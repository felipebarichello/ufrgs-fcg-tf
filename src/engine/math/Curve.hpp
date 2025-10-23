#pragma once
#include "math/linalg.hpp"
#include <vector>

namespace engine {
    class Curve {
        public:
            virtual Vec3 get_point(float t) = 0;
            virtual Vec3 get_tangent(float t) = 0;
    };
} // namespace engine

