#pragma once

#include <engine>
#include <memory>
#include <game/components/player/PlayerShipController.hpp>

namespace game::components {
    class ShipCameraController : public engine::Behavior {
        public:
            ShipCameraController(PlayerShipController* player_controller, engine::Camera* camera) : ship_controller(player_controller), camera(camera) {}
            void Update() override;

        private:
            PlayerShipController* ship_controller;
            engine::Camera* camera;

            float camera_rotation_smooth = 0.1f;
            float vertical_offset = 1.0f;
            float default_distance = 10.0f;
    };
} // namespace game::components
