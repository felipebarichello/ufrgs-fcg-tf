#pragma once

#include <engine>
#include <memory>
#include <vector>
#include <optional>
#include <game/components/player/HumanoidPlayerController.hpp>
// Forward declare WalkerController
namespace game::components { class WalkerController; }

namespace game::components {
    class GroundEnemyController : public engine::Behavior {
        public:
            /// @param target The VObject the enemy should try to follow (usually the player VObject)
            /// @param planets List of planets for gravity/grounding calculations
            GroundEnemyController(HumanoidPlayerController* target_controller, std::vector<PlanetInfo*> planets) : target_controller(target_controller), planets(planets) {}

            void Awake() override;
            void Update() override;

        private:
            // Target references
            HumanoidPlayerController* target_controller = nullptr;
            engine::VObject* target_vobj = nullptr;

            // AI: planets available for any future needs
            std::vector<PlanetInfo*> planets;

            // Walker component used to perform physics for this enemy
            WalkerController* walker = nullptr;

            // Internal helpers
            void update_follow_target();
    };
} // namespace game::components
