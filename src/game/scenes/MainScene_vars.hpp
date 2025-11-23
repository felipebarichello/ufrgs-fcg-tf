#pragma once

#include <vector>
#include <game/components/PlanetInfo.hpp>
#include <engine/vobject/components/TextDrawable.hpp>

// Forward declarations
namespace game::components {
    class PlayerController;
}

namespace game::scenes::main_scene {
    inline game::components::PlayerController* player;
    inline std::vector<components::PlanetInfo*> planets;
    inline engine::TextDrawable* game_over_text;
}
