#pragma once
#include "math/linalg.hpp"
#include <vector>

namespace engine {
    class Curve {
        public:
            virtual Vec3 get_point(float t) const = 0;
            virtual Vec3 get_tangent(float t) const = 0;
    };
} // namespace engine