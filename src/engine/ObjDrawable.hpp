#pragma once
#include <tiny_obj_loader.h>
#include <string>
#include <stdexcept>
#include <set>
#include <vector>
#include <glm/glm.hpp>
#include "vao.hpp"
#include "math/matrices.hpp"
#include "Drawable.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace engine {

    class ObjDrawable : public Drawable {
    public:
        ObjDrawable(engine::Vao* vao);
        ~ObjDrawable() = default;
        void draw(GLuint program_id) override;
        void set_diffuse_reflectance(glm::vec3 reflectance);
        void set_specular_reflectance(glm::vec3 reflectance);
        void set_enviornment_reflectance(glm::vec3 reflectance);
        void set_specular_exponent(float exponent);
        void set_position(glm::vec3 position);
        void set_rotation(float rotation_angle, glm::vec3 rotation_axis);
        void set_scale(glm::vec3 scale);
    private:
        engine::Vao* vao_ptr;
        glm::vec3 diffuse_reflectance{0.8f, 0.4f, 0.8f};
        glm::vec3 specular_reflectance{0.8f, 0.8f, 0.8f};
        glm::vec3 enviornment_reflectance{0.04f, 0.2f, 0.4f};
        float specular_exponent{32.0f};
        void update_model_matrix();
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 rotation_axis{0.0f, 1.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        float rotation_angle{0.0f};
        glm::mat4 model{1.0f};
    };
}