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
    };
} // namespace game::components
