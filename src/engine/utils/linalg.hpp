#pragma once 

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace engine {
    using Vec3 = glm::vec3;
    using Mat4 = glm::mat4;

    Mat4 identity_matrix();
}
