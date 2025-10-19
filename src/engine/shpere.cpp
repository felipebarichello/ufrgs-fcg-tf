#include "sphere.hpp"

Sphere::Sphere() {
    this->position = glm::vec3(0.0f);
    this->rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->scale = glm::vec3(1.0f);
    this->rotation_angle = 0.0f;

    this->update_model_matrix();
    this->vao = this->build_vao();
}

void Sphere::set_position(const glm::vec3 position) {
    this->position = position;
    this->update_model_matrix();
}

void Sphere::set_rotation(float rotation_angle, const glm::vec3 rotation_axis) {
    this->rotation_angle = rotation_angle;
    this->rotation_axis = rotation_axis;
    this->update_model_matrix();
}

void Sphere::set_scale(const glm::vec3 scale) {
    this->scale = scale;
    this->update_model_matrix();
}

void Sphere::update_model_matrix() {
    using namespace engine;
    this->model = Matrix_Translate(this->position.x, this->position.y, this->position.z) // TERCEIRO translação
            * Matrix_Rotate(this->rotation_angle, glm::vec4(this->rotation_axis, 0.0f)) // SEGUNDO rotação
            * Matrix_Scale(this->scale.x, this->scale.y, this->scale.z); // PRIMEIRO escala
}

void Sphere::draw(GLint g_model_uniform) {
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(this->model));
    this->vao.draw();
}

engine::Vao Sphere::build_vao() {
    // Build and return the VAO for the sphere
    std::vector<GLfloat> point_positions;
    std::vector<GLfloat> point_colors;
    std::vector<GLuint> point_indices;

    const float pi = 3.14159265358979323846f;

    float delta_phi = pi / (float) this->parallels;
    float delta_theta = 2.0f * pi / (float) this->meridians;
    
    float phi = -pi/2;
    float theta = 0;

    for (int i = 0; i < this->meridians; i++) {
        for (int j = 0; j < this->parallels; j++) {
            float x = this->ray*cos_theta*sin_phi;
            float y = this->ray*sin_theta*sin_phi;
        }
    }
}
