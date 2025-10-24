// Matrix helpers declarations. Implementations are provided in matrices.cpp

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <cstdio>

namespace engine {
    glm::mat4 h_line_matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );

    glm::mat4 h_identity_matrix();
    glm::mat4 h_translate_matrix(float tx, float ty, float tz);
    glm::mat4 h_scale_matrix(float sx, float sy, float sz);
    glm::mat4 h_rotate_x_matrix(float angle);
    glm::mat4 h_rotate_y_matrix(float angle);
    glm::mat4 h_rotate_z_matrix(float angle);
    glm::mat4 h_matrix_rotate(float angle, glm::vec4 axis);

    float h_norm(glm::vec4 v);
    float h_norm(glm::vec3 v);

    glm::vec4 h_cross_product(glm::vec4 u, glm::vec4 v);
    float h_dot_product(glm::vec4 u, glm::vec4 v);

    glm::mat4 h_ortographic_matrix(float l, float r, float b, float t, float n, float f);
}
