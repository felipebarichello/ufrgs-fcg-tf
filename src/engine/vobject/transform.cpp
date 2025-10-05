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

    void Transform::set_matrix(Mat4 matrix) {
        this->matrix = matrix;
    }

    void Transform::set_basis_vectors(Vec3 u, Vec3 v, Vec3 w) {
        this->matrix[0] = glm::vec4(u, 0.0f);
        this->matrix[1] = glm::vec4(v, 0.0f);
        this->matrix[2] = glm::vec4(w, 0.0f);
    }

} // namespace engine
