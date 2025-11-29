#pragma once

#include <vector>
#include <engine>
#include <game/components/PlanetInfo.hpp>
#include <game/components/physics/KinematicBody.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    
    class Gravity : public engine::Behavior {
        public:
            // Gravity / grounding
            // static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            static constexpr float GRAVITATIONAL_CONSTANT = 0.0f;

            Gravity(KinematicBody* kinematic) : kinematic(kinematic), planets(scenes::main_scene::planets) {}
            // Gravity(KinematicBody* kinematic, std::vector<PlanetInfo*> planets) : kinematic(kinematic), planets(planets) {}
            void Update() override;
            engine::Vec3 compute_equivalent_gravity();

        private:
            KinematicBody* kinematic;
            std::vector<PlanetInfo*> planets;
    };
    
} // namespace game::components
