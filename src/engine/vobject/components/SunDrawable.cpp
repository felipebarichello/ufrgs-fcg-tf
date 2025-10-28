#include "SunDrawable.hpp"
#include "ObjLoader.hpp"
#include "Camera.hpp"
#include <macros>
#include <iostream>
#include <stdexcept>

using namespace engine;

SunDrawable::SunDrawable(std::string file_name) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
}

SunDrawable::SunDrawable(std::string file_name, glm::vec3 sun_color, float base_intensity, float fresnel_intensity, float fresnel_power) {
    std::string model_path = EngineController::get_executable_directory() + "/../../assets/" + file_name;
    this->vao_ptr = ObjLoader::load(model_path.c_str());
    this->sun_color = sun_color;
    this->base_intensity = base_intensity;
    this->fresnel_intensity = fresnel_intensity;
    this->fresnel_power = fresnel_power;
}

SunDrawable::~SunDrawable() = default;

void SunDrawable::draw() {
    GLuint program_id = EngineController::get_sun_program_id();
    glUseProgram(program_id);

    // Set sun shader uniforms
    GLint sun_color_uniform = glGetUniformLocation(program_id, "sun_color");
    GLint base_intensity_uniform = glGetUniformLocation(program_id, "base_intensity");
    GLint fresnel_intensity_uniform = glGetUniformLocation(program_id, "fresnel_intensity");
    GLint fresnel_power_uniform = glGetUniformLocation(program_id, "fresnel_power");
    
    glUniform3fv(sun_color_uniform, 1, &this->sun_color[0]);
    glUniform1f(base_intensity_uniform, this->base_intensity);
    glUniform1f(fresnel_intensity_uniform, this->fresnel_intensity);
    glUniform1f(fresnel_power_uniform, this->fresnel_power);

    // Set view matrix
    GLint view_uniform = glGetUniformLocation(program_id, "view");
    glm::mat4 view_matrix = engine::Camera::get_main()->get_view_matrix();
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix));

    // Set projection matrix
    GLint projection_uniform = glGetUniformLocation(program_id, "projection");
    glm::mat4 projection_matrix = engine::Camera::get_main()->get_perspective_matrix();
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    // Set model matrix
    glm::mat4 model_matrix = this->get_vobject()->transform().get_model_matrix();
    GLint model_uniform = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));

    if (this->vao_ptr) {
        this->vao_ptr->draw();
    }
}
