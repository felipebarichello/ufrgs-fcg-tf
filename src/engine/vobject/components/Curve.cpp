#include "Curve.hpp"

using namespace engine;

Curve::Curve(const std::vector<Vec3> pts) {
    if (pts.size() != 4) {
        throw std::invalid_argument("Curve requires exactly 4 control points.");
    }
    this->points = pts;
}

Vec3 Curve::get_point(float t) {
    Vec3 p0 = points[0];
    Vec3 p1 = points[1];
    Vec3 p2 = points[2];
    Vec3 p3 = points[3];
    return  (1-t)*(1-t)*(1-t)*p0 +
                3*(1-t)*(1-t)*p1 +
                  3*t*t*(1-t)*p2 +
                        t*t*t*p3;
}

Vec3 Curve::get_tangent(float t) {
    Vec3 p0 = points[0];
    Vec3 p1 = points[1];
    Vec3 p2 = points[2];
    Vec3 p3 = points[3];
    return 3*(1-t)*(1-t)*(p1 - p0) +
               6*(1-t)*t*(p2 - p1) +
                   3*t*t*(p3 - p2);
}
