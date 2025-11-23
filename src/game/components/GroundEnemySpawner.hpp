#pragma once

#include <engine>
#include <game/components/PlanetInfo.hpp>

namespace game::components {
    class GroundEnemySpawner : public engine::Behavior {
        public:
            GroundEnemySpawner(PlanetInfo* planet) : planet(planet) {}
            void Start() override;

        private:
            PlanetInfo* planet;
    };
}
