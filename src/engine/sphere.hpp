#pragma once

#include "vao.hpp"
#include "math/matrices.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Sphere {
private:
    glm::mat4 model;
    glm::vec3 position;
    glm::vec3 rotation_axis;
    glm::vec3 scale;
    engine::Vao vao = engine::Vao();
    float rotation_angle;
    void update_model_matrix();
    engine::Vao build_vao();
    size_t parallels = 5;
    size_t meridians = 5;
    float radius = 0.5f;
public:
    Sphere();
    void set_position(const glm::vec3 position);
    void set_rotation(float rotation_angle, const glm::vec3 rotation_axis);
    void set_scale(const glm::vec3 scale);
    void draw(GLint g_model_uniform);
};