#pragma once

#include <engine>
#include <memory>

using engine::Transform;

namespace game::components {
    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(engine::Camera* camera, float planet_radius) : camera(camera), planet_radius(planet_radius) {}
            void Start() override;
            void Update() override;
            float speed = 10.0f;
            
            // Fuel system
            float get_fuel() const { return fuel; }
            float get_max_fuel() const { return max_fuel; }

        private:
            struct SphericalInput;

            engine::Camera* camera;
            float planet_radius;
            
            Transform stored_child_cam_transform;
            // Camera smoothing parameters
            float camera_smooth_speed = 0.1f; // higher = snappier
            float camera_distance = 10.0f;
            float camera_height = 2.0f; // vertical offset above player
            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            engine::Vec2 secondary_move_vector {0.0f, 0.0f};
            float phi_max = 3.141592f/2;
            float phi_min = -3.141592f/2;

            void update_transform_due_to_input();
            void update_camera();
            engine::Vec3 velocity = engine::Vec3(0.0f);
            float acceleration = 0.0f;
            bool accelerating_forward = false;
            bool accelerating_backward = false;
            float max_speed = 500.0f;
            float min_speed = 10.0f;
            
            // Fuel system
            float fuel = 100.0f;
            float max_fuel = 100.0f;
            float fuel_consumption_rate = 10.0f; // units per second when accelerating
            
            SphericalInput get_spherical_input();
    };
} // namespace game::components
