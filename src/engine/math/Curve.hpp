#pragma once
#include "math/linalg.hpp"
#include <vector>

namespace engine {
    class Curve {
        public:
            Curve() = default;
            Curve(const std::vector<Vec3> pts);
            Vec3 get_point(float t);
            Vec3 get_tangent(float t);
        private:
            std::vector<Vec3> points;
    };
} // namespace engine

