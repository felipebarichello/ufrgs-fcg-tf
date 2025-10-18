#include "cube.hpp"

Cube::Cube() {
    position = glm::vec3(0.0f);
    rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    scale = glm::vec3(1.0f);
    rotation_angle = 0.0f;
    update_model_matrix();
    this->vao = build_vao();
}

void Cube::set_position(const glm::vec3 position) {
    this->position = position;
    update_model_matrix();
}

void Cube::set_rotation(float rotation_angle, const glm::vec3 rotation_axis) {
    this->rotation_angle = rotation_angle;
    this->rotation_axis = rotation_axis;
    update_model_matrix();
}

void Cube::set_scale(const glm::vec3 scale) {
    this->scale = scale;
    update_model_matrix();
}

void Cube::update_model_matrix() {
    using namespace engine;
    this->model = Matrix_Translate(this->position.x, this->position.y, this->position.z) // TERCEIRO translação
            * Matrix_Rotate(this->rotation_angle, glm::vec4(this->rotation_axis, 0.0f)) // SEGUNDO rotação
            * Matrix_Scale(this->scale.x, this->scale.y, this->scale.z); // PRIMEIRO escala
}

void Cube::draw(GLint g_model_uniform) {
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(this->model));
    this->vao.draw();
}

engine::Vao Cube::build_vao() {
    // Build and return the VAO for the cube
    GLfloat face_positions[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
    };

    GLfloat face_colors[] = {
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
    };

    GLuint face_indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 7, 6,
        6, 5, 4,
        // Top face
        0, 3, 7,
        7, 4, 0,
        // Bottom face
        1, 5, 6,
        6, 2, 1,
        // Left face
        0, 4, 5,
        5, 1, 0,
        // Right face
        3, 2, 6,
        6, 7, 3
    };

    return engine::VaoBuilder()
        .add_vbo(0, 4, sizeof(face_positions), face_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(face_colors), face_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(face_indices), face_indices, GL_STATIC_DRAW)
        .build(GL_TRIANGLES, sizeof(face_indices)/sizeof(GLuint), GL_UNSIGNED_INT);

}