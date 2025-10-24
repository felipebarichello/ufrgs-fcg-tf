#pragma once

#include <engine>
#include <memory>

using engine::Transform;
using engine::h_norm;
using engine::h_normalize;

namespace game::components {
    class PlayerController : public engine::Behavior {
        public:
            PlayerController(engine::Camera* camera, float planet_radius) : camera(camera), planet_radius(planet_radius) {}
            void Start() override;
            void Update() override;

        private:
            struct SphericalInput;

            engine::Camera* camera;
            float planet_radius;
            
            Transform stored_child_cam_transform;
            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float speed = 5.0f;
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;
            engine::Vec2 secondary_move_vector {0.0f, 0.0f};
            float phi_max = 3.141592f/2;
            float phi_min = -3.141592f/2;
            float jump_strength = 110.0f;

            void update_velocity_due_to_environment();
            void update_velocity_due_to_input();

            void update_transform();

            void update_released_camera();
            SphericalInput get_spherical_input();
            void set_camera_phi(float new_phi);
            void jump();
            void toggle_camera_release();
            void align_quaternion_to_planet_surface();

            float jumped = false;
    };
} // namespace game::components
