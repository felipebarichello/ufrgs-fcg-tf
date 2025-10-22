#pragma once
#include <engine/vobject/components/Drawable.hpp>
#include "engine/vao.hpp"
#include "engine/EngineController.hpp"
#include <glm/vec3.hpp>

class BunnyDrawable : public engine::Drawable {
    public:
        BunnyDrawable();
        void draw() override;
    private:
        glm::vec3 diffuse_reflectance{0.8f, 0.4f, 0.8f};
        glm::vec3 specular_reflectance{0.8f, 0.8f, 0.8f};
        glm::vec3 enviornment_reflectance{0.04f, 0.2f, 0.4f};
        float specular_exponent{32.0f};
};