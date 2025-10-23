#pragma once
#include "BezierCurve.hpp"
#include <vector>

namespace engine {
    class PieceWiseBezierCurve : public Curve {
        public:
            PieceWiseBezierCurve() = default;
            PieceWiseBezierCurve(const std::vector<BezierCurve>& segments);
            Vec3 get_point(float t) const override;
            Vec3 get_tangent(float t) const override;
        private:
            std::vector<BezierCurve> segments;
            float segment_length;
            size_t segment_count;
    };
} // namespace engine