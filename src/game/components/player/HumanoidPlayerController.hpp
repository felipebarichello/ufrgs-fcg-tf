#pragma once

#include <engine>
#include <memory>
#include <game/components/WalkerController.hpp>
#include <game/components/physics/AngularVelocity.hpp>
#include "SphericalCoords.hpp"

namespace game::components {
    class HumanoidPlayerController : public engine::Behavior {
        public:
            // Now HumanoidPlayerController only handles input and camera.
            // It receives a pointer to a WalkerController component that resides in the same VObject.
            HumanoidPlayerController(engine::Camera* camera, WalkerController* walker, AngularVelocity* angular, engine::CylinderCollider* cylinder_collider) : camera(camera), walker(walker), angular(angular), cylinder_collider(cylinder_collider) {}
            void Start() override;
            void Update() override;
            void PostUpdate() override;
            void OnEnable() override;
            void OnDisable() override;
            
            float get_camera_phi() const { return this->camera_phi; }
            void reset_camera_phi() { this->camera_phi = 0.0f; }

            // Expose walker for external queries (e.g., collision checks)
            WalkerController* get_walker() { return this->walker; }

            engine::CylinderCollider* get_cylinder_collider() { return this->cylinder_collider; }
            
            void hit_by_enemy();

        private:
            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            static constexpr float MIN_SURFACE_ALIGN_DISTANCE = 2.0f;
            static constexpr float MAX_SURFACE_ALIGN_DISTANCE = 200.0f;
            
            engine::Camera* camera;
            // Walker component responsible for all movement physics
            WalkerController* walker = nullptr;
            AngularVelocity* angular = nullptr;

            engine::CylinderCollider* cylinder_collider;

            /// @brief Used both for storing camera transform before releasing and for bobbing.
            engine::Transform stored_child_cam_transform;
            
            // View bobbing
            float bob_timer = 0.0f;
            float bob_frequency = 6.0f; // how fast bob cycles when moving
            float bob_vertical_amplitude = 0.08f; // vertical bob amplitude in local units
            float bob_sway_amplitude = 0.1f; // horizontal sway amplitude
            float bob_min_input_threshold = 0.01f; // ignore tiny inputs

            // Input vector used by the camera and forwarded to WalkerController
            engine::Vec2 move_vector_2d {0.0f, 0.0f};
            float camera_phi = 0.0f;
            float camera_theta = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;
            float released_camera_speed = 5.0f;
            float phi_min = -2.5f/2;
            float phi_max =  2.5f/2;
            float theta_max = 2.0f;

            void handle_input();
            void update_released_camera();
            SphericalCoords get_spherical_input();
            void set_camera_phi(float new_phi);
            void set_camera_theta(float new_theta);
            void update_camera();
            void on_jump_pressed();
            void toggle_camera_release();
    };
} // namespace game::components
