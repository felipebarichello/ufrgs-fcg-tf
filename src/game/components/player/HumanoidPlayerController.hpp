#pragma once

#include <engine>
#include <memory>
#include <game/components/PlanetInfo.hpp>

namespace game::components {
    class HumanoidPlayerController : public engine::Behavior {
        public:
            HumanoidPlayerController(engine::Camera* camera, std::vector<PlanetInfo*> planets) : camera(camera), planets(planets) {}
            void Start() override;
            void Update() override;

        private:
            struct SphericalInput;

            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            static constexpr float MIN_SURFACE_ALIGN_DISTANCE = 2.0f;
            static constexpr float MAX_SURFACE_ALIGN_DISTANCE = 200.0f;

            bool active = true;
            engine::Camera* camera;
            std::vector<PlanetInfo*> planets;
            
            engine::Transform stored_child_cam_transform;
            engine::Vec2 move_vector_2d {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float walk_accel = 50.0f;
            float walk_deaccel = 200.0f;
            float max_walk_speed = 80.0f;
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;
            float released_camera_speed = 5.0f;
            engine::Vec2 secondary_move_vector {0.0f, 0.0f};
            float phi_max = 3.141592f/2;
            float phi_min = -3.141592f/2;
            float jump_strength = 80.0f;
            std::optional<PlanetInfo*> grounded_to = std::nullopt;
            PlanetInfo* closest_planet = nullptr;

            void update_transform_due_to_environment();
            void update_transform_due_to_input();
            void update_released_camera();
            engine::Vec3 compute_equivalent_gravity();
            SphericalInput get_spherical_input();
            void set_camera_phi(float new_phi);
            void jump();
            void toggle_camera_release();
            void correct_planet_collision();
            void align_to_closest_planet();
    };
} // namespace game::components
