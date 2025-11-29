#pragma once

#include <engine>
#include <memory>

namespace game::components {
    class ShipCameraController : public engine::Behavior {
        public:
            ShipCameraController(engine::Camera* camera) : camera(camera) {}
            void Update() override;

        private:
            engine::Camera* camera;

            float camera_rotation_smooth = 0.1f;
            float vertical_offset = 1.0f;
            float default_distance = 10.0f;

            /// @brief Time in seconds for the camera displacement animation when switching to ship
            float animation_time = 1.0f;

            float animation_timer = 0.0f;
    };
} // namespace game::components
