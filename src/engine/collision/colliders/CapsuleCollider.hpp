#pragma once

#include <engine/vobject/Component.hpp>

namespace engine {
    class CapsuleCollider : public Component {
        public:
            CapsuleCollider(float height, float radius) : height(height), radius(radius) {}
            float get_height() const { return this->height; }
            float get_radius() const { return this->radius; }

        private:
            float height;
            float radius;
    };
}
