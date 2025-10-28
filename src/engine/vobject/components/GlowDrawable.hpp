#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class GlowDrawable : public engine::Drawable {
    public:
        GlowDrawable(std::string file_name);
        GlowDrawable(std::string file_name, glm::vec3 glow_color, float glow_intensity, float glow_falloff);
        ~GlowDrawable();
        void draw() override;
    private:
        glm::vec3 glow_color{1.0f, 0.9f, 0.5f};
        float glow_intensity{3.5f};      // Increased for brighter corona
        float glow_falloff{0.3f};        // Lower value = slower falloff = wider corona spread
};
