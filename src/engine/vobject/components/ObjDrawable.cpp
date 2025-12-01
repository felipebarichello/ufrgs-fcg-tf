#include "ObjDrawable.hpp"
#include "ObjLoader.hpp"
#include <macros>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace engine;

ObjDrawable::ObjDrawable(std::string file_name) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
    this->seed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // random seed for this drawable
}

ObjDrawable::ObjDrawable(std::string file_name, std::string texture_file_name, EngineController::ShaderType shader_type) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    std::string texture_path = EngineController::get_executable_directory() + "/../../assets/" + texture_file_name;
    // Constructor: model and optional texture paths provided.
    this->vao_ptr = ObjLoader::load(model_path.c_str(), texture_path.c_str());
    this->shader_type = shader_type;
    this->seed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

ObjDrawable::ObjDrawable(std::string file_name, std::string texture_file_name) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    std::string texture_path = EngineController::get_executable_directory() + "/../../assets/" + texture_file_name;
    // Constructor: model and optional texture paths provided.
    this->vao_ptr = ObjLoader::load(model_path.c_str(), texture_path.c_str());
    this->shader_type = EngineController::ShaderType::Phong;
    this->seed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

ObjDrawable::ObjDrawable(std::string file_name, EngineController::ShaderType shader_type) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    // Constructor: model and optional texture paths provided.
    this->vao_ptr = ObjLoader::load(model_path.c_str());
    this->shader_type = shader_type;
    this->seed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void ObjDrawable::draw() {
    if (!this->visible) return;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    GLuint program_id = EngineController::get_program_id(this->shader_type);
    glUseProgram(program_id);
    GLint tex_loc = glGetUniformLocation(program_id, "TextureImage");
    if (tex_loc != -1) {
        glUniform1i(tex_loc, this->vao_ptr->texture_id);
    }

    engine::Mat4 model_matrix = this->get_vobject()->transform().get_model_matrix();
    GLsizei model_uniform = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
    // Send the per-drawable random seed to the shader before drawing
    GLfloat random_seed = this->seed;
    GLint seed_uniform = glGetUniformLocation(program_id, "random_seed");
    if (seed_uniform != -1) {
        glUniform1f(seed_uniform, random_seed);
    }

    if (this->vao_ptr) {
        this->vao_ptr->draw();
    }
}

void ObjDrawable::set_visible() {
    this->visible = true;
}

void ObjDrawable::set_invisible() {
    this->visible = false;
}