#pragma once

#include <engine/vobject/Component.hpp>
#include <cmath>

namespace engine {
    class SphereCollider : public Component {
        public:
            SphereCollider(float radius) : radius_squared(radius * radius) {}
            float get_radius_squared() const { return this->radius_squared; }
            float get_radius() const { return std::sqrt(this->radius_squared); }
            
        private:
            float radius_squared;
    };
}
