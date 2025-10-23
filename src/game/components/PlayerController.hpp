#pragma once

#include <engine>
#include <memory>

namespace game::components {
    class PlayerController : public engine::Behavior {
        public:
            PlayerController(engine::Camera* camera, float planet_radius) : camera(camera), planet_radius(planet_radius) {}
            void Start() override;
            void Update() override;

        private:
            engine::Camera* camera;
            float planet_radius;

            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float speed = 0.1f;
            float camera_phi = 0.0f;
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            bool released_camera = false;

            void update_position();
            void update_direction();
            void toggle_camera_release();
    };
} // namespace game::components
