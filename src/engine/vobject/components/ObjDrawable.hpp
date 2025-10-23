#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class ObjDrawable : public engine::Drawable {
    public:
        ObjDrawable(std::string file_name);
        void draw() override;
    private:
        glm::vec3 diffuse_reflectance{0.8f, 0.4f, 0.8f};
        glm::vec3 specular_reflectance{0.8f, 0.8f, 0.8f};
        glm::vec3 enviornment_reflectance{0.04f, 0.2f, 0.4f};
        float specular_exponent{32.0f};
};