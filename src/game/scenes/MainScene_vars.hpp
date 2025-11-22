#pragma once

#include <vector>
#include <game/components/PlanetInfo.hpp>

// Forward declarations
namespace game::components {
    class HumanoidPlayerController;
}

namespace game::scenes::main_scene {
    inline components::HumanoidPlayerController* player;
    inline std::vector<components::PlanetInfo*> planets;
}
