#include "matrices.hpp"
#include <engine>

namespace engine {
    // Implementations
    glm::mat4 h_line_matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        return glm::mat4(
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        );
    }

    glm::mat4 h_identity_matrix() {
        return h_line_matrix(
            1.0f , 0.0f , 0.0f , 0.0f ,
            0.0f , 1.0f , 0.0f , 0.0f ,
            0.0f , 0.0f , 1.0f , 0.0f ,
            0.0f , 0.0f , 0.0f , 1.0f
        );
    }

    glm::mat4 h_translate_matrix(float tx, float ty, float tz) {
        return h_line_matrix(
            1.0f, 0.0f, 0.0f, tx,
            0.0f, 1.0f, 0.0f, ty,
            0.0f, 0.0f, 1.0f, tz,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    glm::mat4 h_scale_matrix(float sx, float sy, float sz) {
        return h_line_matrix(
            sx,   0.0f, 0.0f, 0.0f,
            0.0f, sy,   0.0f, 0.0f,
            0.0f, 0.0f, sz,   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    glm::mat4 h_rotate_x_matrix(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return h_line_matrix(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f,   c,  -s,   0.0f,
            0.0f,   s,   c,   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    glm::mat4 h_rotate_y_matrix(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return h_line_matrix(
            c,    0.0f,  s,    0.0f,
            0.0f, 1.0f,  0.0f, 0.0f,
            -s,   0.0f,  c,    0.0f,
            0.0f, 0.0f,  0.0f, 1.0f
        );
    }

    glm::mat4 h_rotate_z_matrix(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return h_line_matrix(
            c,   -s,   0.0f, 0.0f,
            s,    c,   0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    float h_norm(engine::Vec4 v)
    {
        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        return sqrt( vx*vx + vy*vy + vz*vz );
    }

    float h_norm(glm::vec3 v)
    {
        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        return sqrt( vx*vx + vy*vy + vz*vz );
    }

    float h_norm(glm::vec2 v)
    {
        float vx = v.x;
        float vy = v.y;

        return sqrt( vx*vx + vy*vy );
    }

    Vec2 h_normalize(Vec2 v)
    {
        float norm = h_norm(v);
        return Vec2( v.x / norm, v.y / norm );
    }

    Vec3 h_normalize(Vec3 v)
    {
        float norm = h_norm(v);
        return Vec3( v.x / norm, v.y / norm, v.z / norm );
    }

    Vec4 h_normalize(Vec4 v)
    {
        float norm = h_norm(v);
        return Vec4( v.x / norm, v.y / norm, v.z / norm, 0.0f );
    }

    glm::mat4 h_matrix_rotate(float angle, engine::Vec4 axis)
    {
        float c = cos(angle);
        float s = sin(angle);

        engine::Vec4 v = normalize(axis);

        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        return h_line_matrix(
            vx*vx*(1-c)+c,      vx*vy*(1-c)-vz*s, vx*vz*(1-c)+vy*s, 0.0f,
            vy*vx*(1-c)+vz*s,   vy*vy*(1-c)+c,    vy*vz*(1-c)-vx*s, 0.0f,
            vz*vx*(1-c)-vy*s,   vz*vy*(1-c)+vx*s, vz*vz*(1-c)+c,    0.0f,
            0.0f,               0.0f,             0.0f,             1.0f
        );
    }

    engine::Vec4 h_cross_product(engine::Vec4 u, engine::Vec4 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;

        return engine::Vec4(
            u2*v3 - u3*v2,
            u3*v1 - u1*v3,
            u1*v2 - u2*v1,
            0.0f
        );
    }

    glm::vec3 h_cross_product(glm::vec3 u, glm::vec3 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;

        return glm::vec3(
            u2*v3 - u3*v2,
            u3*v1 - u1*v3,
            u1*v2 - u2*v1
        );
    }

    float h_dot_product(engine::Vec4 u, engine::Vec4 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float u4 = u.w;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;
        float v4 = v.w;

        if ( u4 != 0.0f || v4 != 0.0f )
        {
            fprintf(stderr, "ERROR: Produto escalar não definido para pontos.\n");
            std::exit(EXIT_FAILURE);
        }

        return u1*v1 + u2*v2 + u3*v3;
    }

    float h_dot_product(glm::vec3 u, glm::vec3 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;

        return u1*v1 + u2*v2 + u3*v3;
    }

    float h_dot_product(glm::vec2 u, glm::vec2 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float v1 = v.x;
        float v2 = v.y;

        return u1*v1 + u2*v2;
    }

    glm::mat4 h_ortographic_matrix(float l, float r, float b, float t, float n, float f)
    {
        glm::mat4 M = h_line_matrix(
            2.0f/(r-l), 0.0f,       0.0f,       -(r+l)/(r-l),
            0.0f,       2.0f/(t-b), 0.0f,       -(t+b)/(t-b),
            0.0f,       0.0f,       2.0f/(f-n), -(f+n)/(f-n),
            0.0f,       0.0f,       0.0f,       1.0f
        );

        return M;
    }
}
