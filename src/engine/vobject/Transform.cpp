
#include "Transform.hpp"
using namespace engine;

Transform::Transform() {
    update_model_matrix();
}

void Transform::set_position(glm::vec3 position) {
    this->position = position;
    update_model_matrix();
}

void Transform::set_rotation(float rotation_angle, glm::vec3 rotation_axis) {
    this->rotation_angle = rotation_angle;
    this->rotation_axis = rotation_axis;
    update_model_matrix();
}

void Transform::set_scale(glm::vec3 scale) {
    this->scale = scale;
    update_model_matrix();
}

void Transform::set_scale(float uniform_scale) {
    this->scale = glm::vec3(uniform_scale, uniform_scale, uniform_scale);
    update_model_matrix();
}

glm::mat4 Transform::get_model_matrix() const {
    return this->model;
}

void Transform::update_model_matrix() {
    this->model = Matrix_Translate(this->position.x, this->position.y, this->position.z) // TERCEIRO translação
            * Matrix_Rotate(this->rotation_angle, glm::vec4(this->rotation_axis, 0.0f)) // SEGUNDO rotação
            * Matrix_Scale(this->scale.x, this->scale.y, this->scale.z); // PRIMEIRO escala
}