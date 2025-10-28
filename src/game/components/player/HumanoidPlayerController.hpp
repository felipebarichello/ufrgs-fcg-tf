#pragma once

#include <engine>
#include <memory>
#include <game/components/PlanetInfo.hpp>
// Forward declare WalkerController to avoid include cycle
namespace game::components { class WalkerController; }

namespace game::components {
    class HumanoidPlayerController : public engine::Behavior {
        public:
            // Now HumanoidPlayerController only handles input and camera.
            // It receives a pointer to a WalkerController component that resides in the same VObject.
            HumanoidPlayerController(engine::Camera* camera, WalkerController* walker) : camera(camera), walker(walker) {}
            void Start() override;
            void Update() override;

        private:
            struct SphericalInput;

            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            static constexpr float MIN_SURFACE_ALIGN_DISTANCE = 2.0f;
            static constexpr float MAX_SURFACE_ALIGN_DISTANCE = 200.0f;

            bool active = true;
            engine::Camera* camera;
            // Walker component responsible for all movement physics
            WalkerController* walker = nullptr;

            engine::Transform stored_child_cam_transform;
            // Input vector used by the camera and forwarded to WalkerController
            engine::Vec2 move_vector_2d {0.0f, 0.0f};
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;
            float released_camera_speed = 5.0f;
            float phi_max = 3.141592f/2;
            float phi_min = -3.141592f/2;
            void update_transform_due_to_input();
            void update_released_camera();
            SphericalInput get_spherical_input();
            void set_camera_phi(float new_phi);
            void on_jump_pressed();
            void toggle_camera_release();
    };
} // namespace game::components
