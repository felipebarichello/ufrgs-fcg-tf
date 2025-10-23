#pragma once 

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <ostream>

namespace engine {
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    Mat4 identity_matrix();
    Vec3 negate(Vec3 v);
    Mat4 invert_orthonormal_matrix(const Mat4& m);

    struct HyperVec3 {
        Vec3 x;
        Vec3 y;
        Vec3 z;

        HyperVec3() = default;
        HyperVec3(Vec3 x, Vec3 y, Vec3 z) : x(x), y(y), z(z) {}
    };

    std::ostream& operator<<(std::ostream& os, const Mat4& m);
}
