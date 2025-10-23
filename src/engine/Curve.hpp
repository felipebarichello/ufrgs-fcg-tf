#pragma once
#include "math/linalg.hpp"
#include <vector>

namespace engine {
    class Curve {
        public:
            Curve(const std::vector<Vec3> pts);
        private:
            std::vector<Vec3> points;
            Vec3 get_point(float t);
            Vec3 get_tangent(float t);
    };
} // namespace engine

