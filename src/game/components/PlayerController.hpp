#pragma once

#include <engine>
#include <memory>

namespace game::components {
    class PlayerController : public engine::Behavior {
        public:
            PlayerController(engine::Camera* camera) : camera(camera) {}
            void Start() override;
            void Update() override;

        private:
            engine::Camera* camera;

            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float speed = 0.1f;
            float camera_theta = 0.0f;
            float camera_phi = 0.0f;
            float v_sensitivity = 0.1f;
            float h_sensitivity = 0.001f;

            void update_position();
            void update_direction();
    };
} // namespace game::components
