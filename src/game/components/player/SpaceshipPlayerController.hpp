#pragma once

#include <engine>
#include <memory>

using engine::Transform;

namespace game::components {
    class SpaceshipPlayerController : public engine::Behavior {
        public:
            SpaceshipPlayerController(engine::Camera* camera, float planet_radius) : camera(camera), planet_radius(planet_radius) {}
            void Start() override;
            void Update() override;
            float speed = 100.0f;

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
            SphericalInput get_spherical_input();
    };
} // namespace game::components
