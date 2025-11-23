#pragma once
#include <engine>

namespace game::components {
    class AngularVelocity : public engine::Behavior {
        public:
            AngularVelocity() = default;
            void Update() override;
            float& ang_velocity() { return this->_ang_velocity; }

        private:
            float _ang_velocity = 0.0f;
    };
}
