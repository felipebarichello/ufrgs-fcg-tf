#pragma once
#include <engine>

namespace game::components {
    class KinematicBody : public engine::Behavior {
        public:
            KinematicBody() = default;
            void Update() override;
            engine::Vec3& get_velocity() { return this->velocity; }
            void set_velocity(const engine::Vec3& vel) { this->velocity = vel; }

        private:
            engine::Vec3 velocity {0.0f, 0.0f, 0.0f};
    };
}
