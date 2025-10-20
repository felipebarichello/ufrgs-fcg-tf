#pragma once

#include "vao.hpp"
#include <engine/math/matrices.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Drawable.hpp"

class Cube : public engine::Drawable {
private:
    glm::mat4 model;
    glm::vec3 position;
    glm::vec3 rotation_axis;
    glm::vec3 scale;
    engine::Vao faces_vao = engine::Vao();
    engine::Vao edges_vao = engine::Vao();
    engine::Vao axes_vao = engine::Vao();
    float rotation_angle;
    void update_model_matrix();
    engine::Vao build_faces_vao();
    engine::Vao build_edges_vao();
    engine::Vao build_axes_vao();
public:
    Cube();
    void set_position(const glm::vec3 position);
    void set_rotation(float rotation_angle, const glm::vec3 rotation_axis);
    void set_scale(const glm::vec3 scale);
    void draw(GLuint program_id);
};