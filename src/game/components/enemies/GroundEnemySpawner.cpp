#include "GroundEnemySpawner.hpp"
#include <engine>
#include <game/instantiators/Enemy.hpp>
#include <engine/math/random.hpp>
#include <game/scenes/MainScene_vars.hpp>

using namespace engine;

namespace game::components {

    void GroundEnemySpawner::PostUpdate() {
        constexpr float DISTANCE_FROM_SURFACE = 2.0f;

        // Spawn an enemy on the planet surface
        PlanetInfo* planet = this->planet;
        float planet_radius = planet->get_radius();

        // Random position on sphere
        float theta = random_float(0.0f, 2.0f * 3.14159265f);
        float phi = random_float(0.0f, 3.14159265f);

        Vec3 dir;
        dir.x = std::sin(phi) * std::cos(theta);
        dir.y = std::sin(phi) * std::sin(theta);
        dir.z = std::cos(phi);

        Vec3 relative_spawn_pos = dir * (planet_radius + DISTANCE_FROM_SURFACE);
        VObject* planet_vobj = planet->get_vobject();
        Vec3 planet_pos = planet_vobj->transform().get_world_position();
        Vec3 spawn_pos = planet_pos + relative_spawn_pos;

        VObjectConfig enemy_config = instantiators::Enemy({
            .home = planet,
        });

        VObject* inst = planet_vobj->get_scene()->instantiate(enemy_config);
        inst->transform().set_world_position(spawn_pos);

        this->disable(); // Spawn only once
    }

} // namespace game::components
