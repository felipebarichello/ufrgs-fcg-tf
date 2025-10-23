#include <engine>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/math/linalg.hpp>
#include <macros>

using engine::EngineController;

namespace engine {
    Mat4 Camera::get_perspective_matrix() const {
        float n = this->near_distance;
        float f = this->far_distance;

        float t = -n * tanf(this->fov / 2.0f);
        float b = -t;
        float r = t * EngineController::get_instance()->get_screen_ratio();
        float l = -r;

        Mat4 m_matrix = Matrix(
            n,    0.0f, 0.0f, 0.0f,
            0.0f, n,    0.0f, 0.0f,
            0.0f, 0.0f, n+f,  -f*n,
            0.0f, 0.0f, 1.0f, 0.0f
        );

        Mat4 p_matrix = Matrix_Orthographic(l, r, b, t, n, f);

        return -m_matrix * p_matrix;
    }

    CameraTransform CameraTransform::Identity() {
        return CameraTransform(
            Vec3(0.0f, 0.0f, 0.0f),
            HyperVec3(
                Vec3(1.0f, 0.0f, 0.0f),
                Vec3(0.0f, 1.0f, 0.0f),
                Vec3(0.0f, 0.0f, 1.0f)
            )
        );
    }

    __supress_shadow_warning
    void CameraTransform::set_position(Vec3 position) {
        this->position = position;
    }

    void CameraTransform::set_basis_from_up_view(Vec3 up, Vec3 view) {
        Vec3 w = negate(view);
        Vec3 u = glm::normalize(glm::cross(up, w));
        Vec3 v = glm::cross(w, u); // No need to normalize

        this->basis = HyperVec3(u, v, w);
    }

    Mat4 CameraTransform::get_matrix() {
        Vec3& p = this->position;
        HyperVec3& b = this->basis;

        return Mat4(
            b.x.x,  b.x.y,  b.x.z,  0.0f,
            b.y.x,  b.y.y,  b.y.z,  0.0f,
            b.z.x,  b.z.y,  b.z.z,  0.0f,
            p.x,    p.y,    p.z,    1.0f
        );
    }
}
