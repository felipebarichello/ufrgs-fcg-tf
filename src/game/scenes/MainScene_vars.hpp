#pragma once

#include <vector>
#include <game/components/PlanetInfo.hpp>

// Forward declarations
namespace game::components {
    class PlayerController;
}

namespace game::scenes::main_scene {
    inline components::PlayerController* player;
    inline std::vector<components::PlanetInfo*> planets;
}
