#pragma once

#include <engine>
#include <memory>
#include <game/components/SpaceshipController.hpp>

namespace game::components {
    class SpaceshipCameraController : public engine::Behavior {
        public:
            SpaceshipCameraController(SpaceshipController* player_controller, engine::Camera* camera);
            void Update() override;

        private:
            struct SphericalInput;

            engine::Camera* camera;
            SpaceshipController* ship_controller;

            float camera_smooth_speed = 0.1f;
            float vertical_offset = 2.0f;
            float camera_vel_smooth = 0.1f;
            float default_distance = 10.0f;
            float min_distance = 5.0f;
            
            engine::Vec3 velocity = engine::Vec3(0.0f);
    };
} // namespace game::components
