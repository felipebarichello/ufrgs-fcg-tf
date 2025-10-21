#include "ObjDrawable.hpp"
#include <macros>

using namespace engine;

ObjDrawable::ObjDrawable(Vao* vao_ptr) {
    this->vao_ptr = vao_ptr;
}

void ObjDrawable::draw(GLuint program_id) {
    GLint Kd = glGetUniformLocation(program_id, "diffuse_reflectance");
    GLint Ks = glGetUniformLocation(program_id, "specular_reflectance");
    GLint Ka = glGetUniformLocation(program_id, "enviornment_reflectance");
    GLint q  = glGetUniformLocation(program_id, "specular_exponent");
    glUniform3fv(Kd, 1, &this->diffuse_reflectance[0]);
    glUniform3fv(Ks, 1, &this->specular_reflectance[0]);
    glUniform3fv(Ka, 1, &this->enviornment_reflectance[0]);
    glUniform1f(q, this->specular_exponent);
    this->update_model_matrix();
    GLsizei model_uniform = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(this->model));
    this->vao_ptr->draw();
}

void ObjDrawable::set_diffuse_reflectance(glm::vec3 reflectance) {
    this->diffuse_reflectance = reflectance;
}

void ObjDrawable::set_specular_reflectance(glm::vec3 reflectance) {
    this->specular_reflectance = reflectance;
}

void ObjDrawable::set_enviornment_reflectance(glm::vec3 reflectance) {
    this->enviornment_reflectance = reflectance;
}

void ObjDrawable::set_specular_exponent(float exponent) {
    this->specular_exponent = exponent;
}

__supress_shadow_warning
void ObjDrawable::set_position(glm::vec3 position) {
    this->position = position;
}

__supress_shadow_warning
void ObjDrawable::set_rotation(float rotation_angle, glm::vec3 rotation_axis) {
    this->rotation_angle = rotation_angle;
    this->rotation_axis = rotation_axis;
}

__supress_shadow_warning
void ObjDrawable::set_scale(glm::vec3 scale) {
    this->scale = scale;
}

void ObjDrawable::update_model_matrix() {
    this->model = Matrix_Translate(this->position.x, this->position.y, this->position.z) // TERCEIRO translação
            * Matrix_Rotate(this->rotation_angle, glm::vec4(this->rotation_axis, 0.0f)) // SEGUNDO rotação
            * Matrix_Scale(this->scale.x, this->scale.y, this->scale.z); // PRIMEIRO escala
}