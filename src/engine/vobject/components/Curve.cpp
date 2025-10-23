#include "Curve.hpp"

using namespace engine;

Curve::Curve(const std::vector<Vec3> pts) {
    this->points = pts;
}

Vec3 Curve::get_point(float t) {
    (void)t;
    return Vec3();
}

Vec3 Curve::get_tangent(float t) {
    (void)t;
    return Vec3(); 
}
