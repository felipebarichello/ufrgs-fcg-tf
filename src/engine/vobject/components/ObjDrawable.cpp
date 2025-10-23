#include "ObjDrawable.hpp"
#include "ObjLoader.hpp"
#include <macros>
#include <iostream>
#include <stdexcept>

using namespace engine;

ObjDrawable::ObjDrawable(std::string file_name) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
}

void ObjDrawable::draw() {
    GLuint program_id = EngineController::get_gpu_program_id();
    GLint Kd = glGetUniformLocation(program_id, "diffuse_reflectance");
    GLint Ks = glGetUniformLocation(program_id, "specular_reflectance");
    GLint Ka = glGetUniformLocation(program_id, "enviornment_reflectance");
    GLint q  = glGetUniformLocation(program_id, "specular_exponent");
    glUniform3fv(Kd, 1, &this->diffuse_reflectance[0]);
    glUniform3fv(Ks, 1, &this->specular_reflectance[0]);
    glUniform3fv(Ka, 1, &this->enviornment_reflectance[0]);
    glUniform1f(q, this->specular_exponent);
    glm::mat4 model_matrix = this->get_vobject()->transform().get_model_matrix();
    GLsizei model_uniform = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
    if (this->vao_ptr) {
        //std::cout << "Drawing ObjDrawable" << std::endl;
        this->vao_ptr->draw();
    }
}