#pragma once

#include <engine>
#include <memory>

using engine::Transform;

namespace game::components {
    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(float planet_radius) : planet_radius(planet_radius) {}
            void Start() override;
            void Update() override;
            float speed = 10.0f;

        private:
            struct SphericalInput;

            float planet_radius;
            
            // Camera smoothing parameters
            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            float thrust_power = 200.0f; // meters/s^2 applied when thrusting

            void update_rotation_due_to_input();
            engine::Vec3 velocity = engine::Vec3(0.0f);
            float acceleration2 = 0.0f;
            bool accelerating_forward = false;
            bool accelerating_backward = false;
            SphericalInput get_spherical_input();
    };
} // namespace game::components
