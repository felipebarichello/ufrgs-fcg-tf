#pragma once
#include "math/linalg.hpp"
#include "Curve.hpp"
#include <vector>

namespace engine {
    class CircularCurve : public Curve {
        public:
            CircularCurve() = default;
            CircularCurve(const Vec3& center, const Vec3& normal, float radius);
            Vec3 get_point(float t) override;
            Vec3 get_tangent(float t) override;
        private:
            Vec3 center;
            Vec3 normal;
            float radius;
            // Basis vectors for the circle's plane
            Vec3 u; 
            Vec3 v;
    };
} // namespace engine

