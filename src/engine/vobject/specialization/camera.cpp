#include "camera.hpp"

namespace engine {
    Transform& Camera::transform() {
        return this->_transform;
    }

    void Camera::set_transform(Transform transform) {
        this->_transform = transform;
    }
}
