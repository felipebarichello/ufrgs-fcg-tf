#pragma once

#include <engine>
#include <game/components/include.hpp>
#include <vector>

namespace game::instantiators {
    struct EnemyConfig {
        game::components::PlanetInfo* home;
    };
    
    engine::VObjectConfig EnemyObj();
    engine::VObjectConfig Enemy(EnemyConfig config);
}
