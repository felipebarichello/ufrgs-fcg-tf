#include "GlowDrawable.hpp"
#include "ObjLoader.hpp"
#include "Camera.hpp"
#include <macros>
#include <iostream>
#include <stdexcept>

using namespace engine;

GlowDrawable::GlowDrawable(std::string file_name) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
}

GlowDrawable::GlowDrawable(std::string file_name, glm::vec3 glow_color, float glow_intensity, float glow_falloff) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
    this->glow_color = glow_color;
    this->glow_intensity = glow_intensity;
    this->glow_falloff = glow_falloff;
}

GlowDrawable::~GlowDrawable() = default;

void GlowDrawable::draw() {
    GLuint program_id = EngineController::get_glow_program_id();
    glUseProgram(program_id);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    GLint glow_color_uniform = glGetUniformLocation(program_id, "glow_color");
    GLint glow_intensity_uniform = glGetUniformLocation(program_id, "glow_intensity");
    GLint glow_falloff_uniform = glGetUniformLocation(program_id, "glow_falloff");
    
    glUniform3fv(glow_color_uniform, 1, &this->glow_color[0]);
    glUniform1f(glow_intensity_uniform, this->glow_intensity);
    glUniform1f(glow_falloff_uniform, this->glow_falloff);

    GLint view_uniform = glGetUniformLocation(program_id, "view");
    glm::mat4 view_matrix = engine::Camera::get_main()->get_view_matrix();
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLint projection_uniform = glGetUniformLocation(program_id, "projection");
    glm::mat4 projection_matrix = engine::Camera::get_main()->get_perspective_matrix();
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    glm::mat4 model_matrix = this->get_vobject()->transform().get_model_matrix();
    GLint model_uniform = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));

    if (this->vao_ptr) {
        this->vao_ptr->draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
