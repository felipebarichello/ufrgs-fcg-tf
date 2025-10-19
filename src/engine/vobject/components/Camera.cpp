#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../math/linalg.hpp"


namespace engine {
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
        Vec3& position = this->position;
        HyperVec3& basis = this->basis;

        return Mat4(
            basis.x.x,  basis.x.y,  basis.x.z,  0.0f,
            basis.y.x,  basis.y.y,  basis.y.z,  0.0f,
            basis.z.x,  basis.z.y,  basis.z.z,  0.0f,
            position.x, position.y, position.z, 1.0f
        );
    }
}
