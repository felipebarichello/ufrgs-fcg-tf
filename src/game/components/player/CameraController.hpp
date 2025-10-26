#pragma once

#include <engine>
#include <memory>
#include "SpaceshipPlayerController.hpp"

using engine::Vec3;

using engine::Transform;

namespace game::components {
    class CameraController : public engine::Behavior {
        public:
            CameraController(SpaceshipPlayerController* player_controller, engine::Camera* camera);
            void Update() override;

        private:
            struct SphericalInput;

            engine::Camera* camera;
            SpaceshipPlayerController* player_controller;

            float camera_smooth_speed = 0.1f; 
            Vec3 offset = {0.0, 2.0, 10.0};
    };
} // namespace game::components
