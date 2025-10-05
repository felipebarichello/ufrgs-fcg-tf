#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace engine {
    Camera::Camera() : _transform(Transform::Identity()) {}

    Transform& Camera::transform() {
        return this->_transform;
    }

    void Camera::set_transform(Transform transform) {
        this->_transform = transform;
    }

    void Camera::set_vectors_from_up_view(Vec3 up, Vec3 view) {
        Vec3 w = negate(view);
        Vec3 u = normalize(glm::cross(up, w));
        Vec3 v = glm::cross(w,u);

        this->_transform.set_basis_vectors(u, v, w);
    }
}
