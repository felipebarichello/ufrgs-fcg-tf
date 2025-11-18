#pragma once

#include <engine>
#include <engine/TextRenderer.hpp>
#include <game/components/SpaceshipController.hpp>

namespace game::components {
    class FuelDisplay : public engine::Behavior {
        public:
            FuelDisplay(SpaceshipController* spaceship_controller);
            ~FuelDisplay();
            void Start() override;
            void Update() override;
            
        private:
            SpaceshipController* spaceship_controller;
            engine::TextRenderer* text_renderer;
    };
} // namespace game::components
