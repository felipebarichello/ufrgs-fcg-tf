#pragma once

#include <engine>

namespace game::components {
    class PlanetInfo : public engine::Component {
        public:
            PlanetInfo(float gravity_mass, float radius) : gravity_mass(gravity_mass), radius(radius) {}
            float get_gravity_mass() const { return 0.0f; }
            float get_radius() const { return this->radius; }

        private:
            /// @brief This is the mass for gravity calculations.
            /// Does not need to be the actual mass of the planet.
            float gravity_mass;

            float radius;
    };
} // namespace game::components
