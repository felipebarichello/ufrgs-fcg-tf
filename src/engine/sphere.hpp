#pragma once

#include "vao.hpp"
#include "math/matrices.hpp"
#include "drawable.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Sphere : public engine::Drawable {
private:
    glm::mat4 model;
    glm::vec3 position;
    glm::vec3 rotation_axis;
    glm::vec3 scale;
    engine::Vao vao = engine::Vao();
    float rotation_angle;
    void update_model_matrix();
    engine::Vao build_vao();
    size_t parallels = 50;
    size_t meridians = 100;
    float radius = 0.5f;
public:
    Sphere();
    void set_position(const glm::vec3 position);
    void set_rotation(float rotation_angle, const glm::vec3 rotation_axis);
    void set_scale(const glm::vec3 scale);
    void draw(GLint g_model_uniform);
};