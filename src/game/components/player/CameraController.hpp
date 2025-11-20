#pragma once

#include <engine>
#include <memory>
#include <game/components/SpaceshipController.hpp>

using engine::Vec3;

using engine::Transform;

namespace game::components {
    class SpaceshipCameraController : public engine::Behavior {
        public:
            SpaceshipCameraController(SpaceshipController* player_controller, engine::Camera* camera);
            void Update() override;

        private:
            struct SphericalInput;

            engine::Camera* camera;
            SpaceshipController* player_controller;

            float camera_smooth_speed = 0.1f; 
            Vec3 offset = {0.0, 2.0, 10.0};
    };
} // namespace game::components
