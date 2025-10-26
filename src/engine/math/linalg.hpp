#pragma once 

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <ostream>

namespace engine {
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    Vec3 negate(Vec3 v);
    bool is_zero(Vec3 v);
    Mat4 h_identity_matrix();
    Mat4 invert_orthonormal_matrix(const Mat4& m);

    std::ostream& operator<<(std::ostream& os, const Vec3& v);
    std::ostream& operator<<(std::ostream& os, const Mat4& m);
}
