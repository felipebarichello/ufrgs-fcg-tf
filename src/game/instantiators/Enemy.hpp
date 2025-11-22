#pragma once

#include <engine>
#include <game/components/include.hpp>
#include <vector>

namespace game::instantiators {
    struct EnemyConfig {
        game::components::PlanetInfo* home;
        std::vector<game::components::PlanetInfo*> planets;
        game::components::HumanoidPlayerController* player;
    };
    
    engine::VObjectConfig EnemyObj();
    engine::VObjectConfig Enemy(EnemyConfig config);
}
