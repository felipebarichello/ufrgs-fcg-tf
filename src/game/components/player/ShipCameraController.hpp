#pragma once

#include <engine>
#include <memory>

namespace game::components {
    class ShipCameraController : public engine::Behavior {
        public:
            ShipCameraController(engine::Camera* camera, engine::Camera* humanoid_camera) : camera(camera), humanoid_camera(humanoid_camera) {}
            void PostUpdate() override;
            void OnEnable() override;

            void link_thrusting(bool* is_thrusting_ptr, float* fuel_ptr) {
                this->is_thrusting_ptr = is_thrusting_ptr;
                this->fuel_ptr = fuel_ptr;
            }

        private:
            engine::Camera* camera;
            engine::Camera* humanoid_camera;
            bool* is_thrusting_ptr = nullptr;
            float* fuel_ptr = nullptr;

            float camera_rotation_smooth = 0.1f;
            float vertical_offset = 1.0f;
            float default_distance = 10.0f;
            float max_distance = 20.0f;

            /// @brief Time in seconds for the camera displacement animation when switching to ship
            float animation_time = 0.5f;

            float acceleration_smooth = 0.05f;

            engine::Vec2 animation_bezier_2 = engine::Vec2(0.5f, 0.0f);
            engine::Vec2 animation_bezier_3 = engine::Vec2(0.8f, 0.8f);
            
            float cam_distance = 10.0f;
            float animation_timer = 0.0f;
            bool is_animating = false;      
    };
} // namespace game::components
