#pragma once

#include <engine>
#include <memory>
#include <vector>
#include <optional>
#include <game/components/player/HumanoidPlayerController.hpp>
#include <game/components/WalkerController.hpp>

namespace game::components {
    class GroundEnemyController : public engine::Behavior {
        public:
            GroundEnemyController(WalkerController* walker, HumanoidPlayerController* target_controller) : walker(walker), target_controller(target_controller) {}

            void Awake() override;
            void Update() override;

        private:
            // Walker component used to perform physics for this enemy
            WalkerController* walker = nullptr;

            // Target references
            HumanoidPlayerController* target_controller = nullptr;
            engine::VObject* target_vobj = nullptr;

            // Internal helpers
            void update_follow_target();
    };
} // namespace game::components
