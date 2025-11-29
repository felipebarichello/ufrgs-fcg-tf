#pragma once 

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <sstream>


namespace engine {
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    Vec3 negate(Vec3 v);
    bool is_zero(Vec2 v);
    bool is_zero(Vec3 v);
    Vec3 lerp(const Vec3& start, const Vec3& end, float t);
    Mat4 h_identity_matrix();
    Mat4 invert_orthonormal_matrix(const Mat4& m);

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
    float norm(engine::Vec3 v);
    float norm(engine::Vec2 v);
    Vec3 normalize(Vec3 v);
    Vec4 h_normalize(Vec4 v);
    Vec2 normalize(Vec2 v);

    engine::Vec4 h_cross(engine::Vec4 u, engine::Vec4 v);
    engine::Vec3 cross(engine::Vec3 u, engine::Vec3 v);
    float h_dot_product(engine::Vec4 u, engine::Vec4 v);
    float dot(engine::Vec3 u, engine::Vec3 v);
    float dot(engine::Vec2 u, engine::Vec2 v);

    engine::Mat4 h_ortographic_matrix(float l, float r, float b, float t, float n, float f);

    /// @brief Assumes t in [0, 1]
    /// @param p0 
    /// @param p1 
    /// @param p2 
    /// @param p3 
    /// @param t 
    /// @return 
    engine::Vec3 bezier3(
        const engine::Vec3& p0,
        const engine::Vec3& p1,
        const engine::Vec3& p2,
        const engine::Vec3& p3,
        float t
    );

    std::string to_string(const Vec2& v);
    std::string to_string(const Vec3& v);
    std::string to_string(const Mat4& m);
}
