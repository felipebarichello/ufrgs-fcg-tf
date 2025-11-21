#pragma once

#include <engine>
#include <memory>
#include <game/components/PlanetInfo.hpp>
#include <game/components/WalkerController.hpp>
#include <game/components/SpaceshipController.hpp>
#include <engine/vobject/components/TextDrawable.hpp>


namespace game::components {
    class HumanoidPlayerController : public engine::Behavior {
        public:
            // Now HumanoidPlayerController only handles input and camera.
            // It receives a pointer to a WalkerController component that resides in the same VObject.
            HumanoidPlayerController(engine::Camera* camera, WalkerController* walker, engine::CylinderCollider* cylinder_collider) : camera(camera), walker(walker), cylinder_collider(cylinder_collider) {}
            void Start() override;
            void Update() override;

            void enable();
            void disable();
            bool is_active() const { return this->active; }

            float get_camera_phi() const { return this->camera_phi; }

            // Expose walker for external queries (e.g., collision checks)
            WalkerController* get_walker() { return this->walker; }

            engine::CylinderCollider* get_cylinder_collider() { return this->cylinder_collider; }
            
            // Called by enemies when they hit this player
            void hit_by_enemy();

        private:
            struct SphericalInput;

            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            static constexpr float MIN_SURFACE_ALIGN_DISTANCE = 2.0f;
            static constexpr float MAX_SURFACE_ALIGN_DISTANCE = 200.0f;

            bool active = true;
            engine::Camera* camera;
            // Walker component responsible for all movement physics
            WalkerController* walker = nullptr;

            engine::CylinderCollider* cylinder_collider;

            /// @brief Used both for storing camera transform before releasing and for bobbing.
            engine::Transform stored_child_cam_transform;

            TextDrawable* game_over_text = nullptr;
            
            // View bobbing
            float bob_timer = 0.0f;
            float bob_frequency = 6.0f; // how fast bob cycles when moving
            float bob_vertical_amplitude = 0.08f; // vertical bob amplitude in local units
            float bob_sway_amplitude = 0.1f; // horizontal sway amplitude
            float bob_min_input_threshold = 0.01f; // ignore tiny inputs

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
