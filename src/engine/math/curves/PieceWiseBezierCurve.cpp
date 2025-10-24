#include "PieceWiseBezierCurve.hpp"
#include <stdexcept>
#include <string>
#include <algorithm>

using namespace engine;

PieceWiseBezierCurve::PieceWiseBezierCurve(const std::vector<BezierCurve>& segments) {
    for (size_t i = 1; i < segments.size(); ++i) {
        Vec3 end_point_prev = segments[i - 1].get_point(1.0f);
        Vec3 start_point_curr = segments[i].get_point(0.0f);
        if (end_point_prev != start_point_curr) {
            throw std::invalid_argument("Segments are not continuous at index " + std::to_string(i));
        }
    }
    this->segments = segments;
    this->segment_count = segments.size();
    this->segment_length = 1.0f / segment_count;
}

Vec3 PieceWiseBezierCurve::get_point(float t) const {
    size_t segment_index = std::min(static_cast<size_t>(t / segment_length), segment_count - 1);
    float local_t = (t - segment_index * segment_length) / segment_length;
    return segments[segment_index].get_point(local_t);
}

Vec3 PieceWiseBezierCurve::get_tangent(float t) const {
    size_t segment_index = std::min(static_cast<size_t>(t / segment_length), segment_count - 1);
    float local_t = (t - segment_index * segment_length) / segment_length;
    return segments[segment_index].get_tangent(local_t);
}