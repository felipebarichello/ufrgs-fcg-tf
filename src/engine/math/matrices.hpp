// Matrix helpers declarations. Implementations are provided in matrices.cpp

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <cstdio>
#include "linalg.hpp"

namespace engine {
    engine::Mat4 h_line_matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );

    engine::Mat4 h_identity_matrix();
    engine::Mat4 h_translate_matrix(float tx, float ty, float tz);
    engine::Mat4 h_scale_matrix(float sx, float sy, float sz);
    engine::Mat4 h_rotate_x_matrix(float angle);
    engine::Mat4 h_rotate_y_matrix(float angle);
    engine::Mat4 h_rotate_z_matrix(float angle);
    engine::Mat4 h_matrix_rotate(float angle, engine::Vec4 axis);

    float h_norm(engine::Vec4 v);
    float h_norm(engine::Vec3 v);
    float h_norm(glm::vec2 v);
    Vec3 h_normalize(Vec3 v);
    Vec4 h_normalize(Vec4 v);
    Vec2 h_normalize(Vec2 v);

    engine::Vec4 h_cross_product(engine::Vec4 u, engine::Vec4 v);
    engine::Vec3 h_cross_product(engine::Vec3 u, engine::Vec3 v);
    float h_dot_product(engine::Vec4 u, engine::Vec4 v);
    float h_dot_product(engine::Vec3 u, engine::Vec3 v);
    float h_dot_product(glm::vec2 u, glm::vec2 v);

    engine::Mat4 h_ortographic_matrix(float l, float r, float b, float t, float n, float f);
}
