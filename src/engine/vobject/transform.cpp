#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

    Transform Transform::Identity() {
        Mat4 matrix = identity_matrix();
        return Transform(matrix);
    }

    void Transform::set_position(Vec3 position) {
        this->matrix = glm::translate(identity_matrix(), position) * this->matrix;
    }

    void Transform::set_rotation(float angle_in_radians, Vec3 axis) {
        this->matrix = glm::rotate(identity_matrix(), angle_in_radians, axis) * this->matrix;
    }

    void Transform::set_scale(Vec3 scale) {
        this->matrix = glm::scale(identity_matrix(), scale) * this->matrix;
    }

    Mat4 Transform::get_matrix() {
        return this->matrix;
    }

} // namespace engine
