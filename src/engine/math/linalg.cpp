#include "linalg.hpp"
#include <sstream>

namespace engine {
    Vec3 negate(Vec3 v) {
        return Vec3(-v.x, -v.y, -v.z);
    }

    bool is_zero(Vec2 v) {
        return (v.x == 0.0f) && (v.y == 0.0f);
    }

    bool is_zero(Vec3 v) {
        return (v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f);
    }

    Vec3 lerp(const Vec3& start, const Vec3& end, float t) {
        return start + t * (end - start);
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

    engine::Mat4 h_line_matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        return engine::Mat4(
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        );
    }

    engine::Mat4 h_identity_matrix() {
        return h_line_matrix(
            1.0f , 0.0f , 0.0f , 0.0f ,
            0.0f , 1.0f , 0.0f , 0.0f ,
            0.0f , 0.0f , 1.0f , 0.0f ,
            0.0f , 0.0f , 0.0f , 1.0f
        );
    }

    engine::Mat4 h_translate_matrix(float tx, float ty, float tz) {
        return h_line_matrix(
            1.0f, 0.0f, 0.0f, tx,
            0.0f, 1.0f, 0.0f, ty,
            0.0f, 0.0f, 1.0f, tz,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    engine::Mat4 h_scale_matrix(float sx, float sy, float sz) {
        return h_line_matrix(
            sx,   0.0f, 0.0f, 0.0f,
            0.0f, sy,   0.0f, 0.0f,
            0.0f, 0.0f, sz,   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    engine::Mat4 h_rotate_x_matrix(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return h_line_matrix(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f,   c,  -s,   0.0f,
            0.0f,   s,   c,   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    engine::Mat4 h_rotate_y_matrix(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return h_line_matrix(
            c,    0.0f,  s,    0.0f,
            0.0f, 1.0f,  0.0f, 0.0f,
            -s,   0.0f,  c,    0.0f,
            0.0f, 0.0f,  0.0f, 1.0f
        );
    }

    engine::Mat4 h_rotate_z_matrix(float angle) {
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

    float norm(engine::Vec3 v)
    {
        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        return sqrt( vx*vx + vy*vy + vz*vz );
    }

    float norm(engine::Vec2 v)
    {
        float vx = v.x;
        float vy = v.y;

        return sqrt( vx*vx + vy*vy );
    }

    Vec2 normalize(Vec2 v)
    {
        float v_norm = norm(v);
        return Vec2( v.x / v_norm, v.y / v_norm );
    }

    Vec3 normalize(Vec3 v)
    {
        float v_norm = norm(v);
        return Vec3( v.x / v_norm, v.y / v_norm, v.z / v_norm );
    }

    Vec4 h_normalize(Vec4 v)
    {
        float norm = h_norm(v);
        return Vec4( v.x / norm, v.y / norm, v.z / norm, 0.0f );
    }

    engine::Mat4 h_matrix_rotate(float angle, engine::Vec4 axis)
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

    engine::Vec4 h_cross(engine::Vec4 u, engine::Vec4 v)
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

    engine::Vec3 cross(engine::Vec3 u, engine::Vec3 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;

        return engine::Vec3(
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

    float dot(engine::Vec3 u, engine::Vec3 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float u3 = u.z;
        float v1 = v.x;
        float v2 = v.y;
        float v3 = v.z;

        return u1*v1 + u2*v2 + u3*v3;
    }

    float dot(engine::Vec2 u, engine::Vec2 v)
    {
        float u1 = u.x;
        float u2 = u.y;
        float v1 = v.x;
        float v2 = v.y;

        return u1*v1 + u2*v2;
    }

    engine::Mat4 h_ortographic_matrix(float l, float r, float b, float t, float n, float f)
    {
        engine::Mat4 M = h_line_matrix(
            2.0f/(r-l), 0.0f,       0.0f,       -(r+l)/(r-l),
            0.0f,       2.0f/(t-b), 0.0f,       -(t+b)/(t-b),
            0.0f,       0.0f,       2.0f/(f-n), -(f+n)/(f-n),
            0.0f,       0.0f,       0.0f,       1.0f
        );

        return M;
    }
    
    std::string to_string(const Vec2& v) {
        std::ostringstream os;
        os << "(" << v.x << ", " << v.y << ")";
        return os.str();
    }

    std::string to_string(const Vec3& v) {
        std::ostringstream os;
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os.str();
    }

    std::string to_string(const Mat4& m) {
        std::ostringstream os;
        os << "["
           << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << "; "
           << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << "; "
           << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << "; "
           << m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3]
           << "]";
        return os.str();
    }
}
