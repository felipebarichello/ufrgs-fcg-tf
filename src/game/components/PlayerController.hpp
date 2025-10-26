#pragma once

#include <engine>
#include <memory>
#include "PlanetInfo.hpp"

using engine::Transform;

namespace game::components {
    class PlayerController : public engine::Behavior {
        public:
            PlayerController(engine::Camera* camera, std::vector<PlanetInfo*> planets) : camera(camera), planets(planets) {}
            void Start() override;
            void Update() override;

        private:
            struct SphericalInput;

            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;

            engine::Camera* camera;
            std::vector<PlanetInfo*> planets;
            
            Transform stored_child_cam_transform;
            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float speed = 1.0f;
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;
            float released_camera_speed = 5.0f;
            engine::Vec2 secondary_move_vector {0.0f, 0.0f};
            float phi_max = 3.141592f/2;
            float phi_min = -3.141592f/2;
            float jump_strength = 50.0f;
            std::optional<PlanetInfo*> grounded_to = std::nullopt;

            void update_transform_due_to_environment();
            void update_transform_due_to_input();
            void update_released_camera();
            engine::Vec3 compute_equivalent_gravity();
            SphericalInput get_spherical_input();
            void set_camera_phi(float new_phi);
            void jump();
            void toggle_camera_release();
            void correct_planet_collision();
    };
} // namespace game::components
