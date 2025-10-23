#pragma once
#include "math/linalg.hpp"
#include "Curve.hpp"
#include <vector>

namespace engine {
    class BezierCurve : public Curve {
        public:
            BezierCurve() = default;
            BezierCurve(const std::vector<Vec3> pts);
            Vec3 get_point(float t) override;
            Vec3 get_tangent(float t) override;
        private:
            std::vector<Vec3> points;
    };
} // namespace engine

