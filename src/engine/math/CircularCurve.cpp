#include "CircularCurve.hpp"

using namespace engine;

CircularCurve::CircularCurve(const Vec3& center, const Vec3& normal, float radius) {
    this->center = center;
    this->normal = normalize(normal);
    this->radius = radius;
    this->u = normalize(cross(this->normal, Vec3(0, 1, 0)));
    this->v = normalize(cross(this->normal, this->u));
}

Vec3 CircularCurve::get_point(float t) {
    // Calculate the point on the circular path
    float angle = t * 2 * M_PI; // Full circle
    return center + radius * (u * (float) cos(angle) + v * (float) sin(angle));
}

Vec3 CircularCurve::get_tangent(float t) {
    return normalize(cross(this->normal, get_point(t) - center));
}
