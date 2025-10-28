#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class SunDrawable : public engine::Drawable {
    public:
        SunDrawable(std::string file_name);
        SunDrawable(std::string file_name, glm::vec3 sun_color, float base_intensity, float fresnel_intensity, float fresnel_power);
        ~SunDrawable();
        void draw() override;
    private:
        glm::vec3 sun_color{1.0f, 0.8f, 0.4f};  // Warm yellow-orange color
        float base_intensity{0.8f};              // Base emissive intensity
        float fresnel_intensity{2.5f};           // Fresnel glow intensity
        float fresnel_power{2.5f};               // Fresnel power (controls edge width)
};
