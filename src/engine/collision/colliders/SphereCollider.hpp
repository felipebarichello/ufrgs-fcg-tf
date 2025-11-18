#pragma once

#include <engine/vobject/Component.hpp>

namespace engine {
    class SphereCollider : public Component {
        public:
            SphereCollider(float radius) : radius(radius) {}
            float get_radius() const { return this->radius; }
            
        private:
            float radius;
    };
}
