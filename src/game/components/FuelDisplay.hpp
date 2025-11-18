#pragma once

#include <engine>
#include <game/components/SpaceshipController.hpp>

namespace game::components {
    class FuelDisplay : public engine::Drawable {
        public:
            FuelDisplay(SpaceshipController* spaceship_controller);
            void draw() override;
            
        private:
            SpaceshipController* spaceship_controller;
            void render_simple_hud();
            GLuint hud_vao, hud_vbo;
            bool initialized;
    };
} // namespace game::components
