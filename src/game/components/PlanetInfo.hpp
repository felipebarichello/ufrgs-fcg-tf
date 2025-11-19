#pragma once

#include <engine>
#include <engine/collision/colliders/SphereCollider.hpp>

namespace game::components {
    class PlanetInfo : public engine::Component {
        public:
            PlanetInfo(float gravity_mass, float radius)
                : gravity_mass(gravity_mass), radius(radius), sphere_collider(new engine::SphereCollider(radius)) {}

            float get_gravity_mass() const { return this->gravity_mass; }
            float get_radius() const { return this->radius; }

            engine::SphereCollider* get_sphere_collider() const { return this->sphere_collider; }

        private:
            /// @brief This is the mass for gravity calculations.
            /// Does not need to be the actual mass of the planet.
            float gravity_mass;

            float radius;

            // Collider representing the planet surface
            engine::SphereCollider* sphere_collider;
    };
} // namespace game::components
