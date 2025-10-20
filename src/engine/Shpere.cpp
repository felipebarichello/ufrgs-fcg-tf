#include "Sphere.hpp"
#include <macros>

Sphere::Sphere() {
    this->position = glm::vec3(0.0f);
    this->rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->scale = glm::vec3(1.0f);
    this->rotation_angle = 0.0f;

    this->update_model_matrix();
    this->vao = this->build_vao();
}

__supress_shadow_warning
void Sphere::set_position(const glm::vec3 position) {
    this->position = position;
    this->update_model_matrix();
}

__supress_shadow_warning
void Sphere::set_rotation(float rotation_angle, const glm::vec3 rotation_axis) {
    this->rotation_angle = rotation_angle;
    this->rotation_axis = rotation_axis;
    this->update_model_matrix();
}

__supress_shadow_warning
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
    // Increase point size
    glPointSize(2.0f);
    this->vao.draw();
}

engine::Vao Sphere::build_vao() {
    // Build and return the VAO for the sphere
    std::vector<GLfloat> point_positions;
    std::vector<GLfloat> point_colors;
    std::vector<GLuint> indices;

    const float pi = 3.14159265358979323846f;

    float delta_phi = pi / ((float) this->parallels - 1.0f);
    float delta_theta = 2.0f * pi / ((float) this->meridians - 1.0f);

    float phi = 0;
    float theta = 0;

    float sin_theta, cos_theta, sin_phi, cos_phi;
    float x, y, z;

    GLuint index = 0;

    point_positions.push_back(0.0f);
    point_positions.push_back(this->radius);
    point_positions.push_back(0.0f);
    point_positions.push_back(1.0f);

    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);

    indices.push_back(index++);

    for (GLuint i = 0; i < this->meridians; i++) {

        sin_theta = sin(theta);
        cos_theta = cos(theta);

        for (GLuint j = 0; j < this->parallels; j++) {
            sin_phi = sin(phi);
            cos_phi = cos(phi);

            x = this->radius*cos_theta*sin_phi;
            y = this->radius*cos_phi;
            z = this->radius*sin_theta*sin_phi;

            point_positions.push_back(x);
            point_positions.push_back(y);
            point_positions.push_back(z);
            point_positions.push_back(1.0f);

            point_colors.push_back(1.0f);
            point_colors.push_back(1.0f);
            point_colors.push_back(1.0f);
            point_colors.push_back(1.0f);

            indices.push_back(index++);

            phi = (float) j * delta_phi;
        }

        theta = (float) i * delta_theta;
    }

    point_positions.push_back(0.0f);
    point_positions.push_back(-this->radius);
    point_positions.push_back(0.0f);
    point_positions.push_back(1.0f);

    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);
    point_colors.push_back(1.0f);

    indices.push_back(index++);

    return engine::VaoBuilder()
        .add_vbo(0, 4, point_positions.size() * sizeof(GLfloat), point_positions.data(), GL_STATIC_DRAW)
        .add_vbo(1, 4, point_colors.size() * sizeof(GLfloat), point_colors.data(), GL_STATIC_DRAW)
        .add_ebo(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW)
        .build(GL_POINTS, (GLsizei)indices.size(), GL_UNSIGNED_INT);
}
