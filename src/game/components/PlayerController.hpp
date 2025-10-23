#pragma once

#include <engine>
#include <memory>

namespace game::components {
    class PlayerController : public engine::Behavior {
        public:
            PlayerController() = default;
            void Start() override;
            void Update() override;

        private:
            engine::Transform* transform;
            engine::Vec2 move_vector {0.0f, 0.0f};
            float speed = 0.1f;
            float camera_theta = 0.0f;
            float camera_phi = 0.0f;
            float sensitivity = 0.1f;

            void update_position();
            void update_direction();
    };
} // namespace game::components
