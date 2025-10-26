#include "linalg.hpp"

namespace engine {
    Vec3 negate(Vec3 v) {
        return Vec3(-v.x, -v.y, -v.z);
    }

    bool is_zero(Vec3 v) {
        return (v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f);
    }

    Mat4 identity_matrix() {
        return Mat4(1.0f);
    }

    Mat4 invert_orthonormal_matrix(const Mat4& m) {
        Vec3 u = Vec3(m[0][0], m[0][1], m[0][2]);
        Vec3 v = Vec3(m[1][0], m[1][1], m[1][2]);
        Vec3 w = Vec3(m[2][0], m[2][1], m[2][2]);
        Vec3 p = Vec3(m[3][0], m[3][1], m[3][2]);
        
        return Mat4(
            u.x, v.x, w.x, 0.0f,
            u.y, v.y, w.y, 0.0f,
            u.z, v.z, w.z, 0.0f,
            -dot(u, p), -dot(v, p), -dot(w, p), 1.0f
        );
    }

    std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Mat4& m) {
        os << "["
           << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << "; "
           << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << "; "
           << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << "; "
           << m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3]
           << "]";
        return os;
    }
}
