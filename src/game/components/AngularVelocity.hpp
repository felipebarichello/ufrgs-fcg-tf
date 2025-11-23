#pragma once
#include <engine>

namespace game::components {
    class AngularVelocity : public engine::Behavior {
        public:
            AngularVelocity() = default;
            void Update() override;
            engine::Vec3& euler_angles() { return this->_euler_angles; }

        private:
            engine::Vec3 _euler_angles = engine::Vec3(0.0f);
    };
}
