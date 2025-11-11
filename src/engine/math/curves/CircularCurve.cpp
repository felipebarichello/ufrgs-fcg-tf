#include "CircularCurve.hpp"

using namespace engine;

CircularCurve::CircularCurve(const Vec3& center, const Vec3& normal, float radius) {
    this->center = center;
    this->normal = normalize(normal);
    this->radius = radius;
    
    Vec3 ref = Vec3(0.0f, 1.0f, 0.0f);
    if (std::fabs(dot(this->normal, ref)) > 0.9999f) {
        ref = Vec3(1.0f, 0.0f, 0.0f);
    }
    
    this->u = normalize(cross(this->normal, ref));
    this->v = normalize(cross(this->normal, this->u));
}

Vec3 CircularCurve::get_point(float t) const {
    // Calculate the point on the circular path
    float angle = t * 2.0f * M_PI; // Full circle
    return center + radius * (u * (float) cos(angle) + v * (float) sin(angle));
}

Vec3 CircularCurve::get_tangent(float t) const {
    return normalize(cross(this->normal, get_point(t) - center));
}
