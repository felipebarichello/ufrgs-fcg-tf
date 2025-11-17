#pragma once

#include <engine>
#include <game/components/SpaceshipController.hpp>

namespace game::components {
    class FuelDisplay : public engine::Behavior {
        public:
            FuelDisplay(SpaceshipController* spaceship_controller);
            void Update() override;
            
        private:
            SpaceshipController* spaceship_controller;
    };
} // namespace game::components
