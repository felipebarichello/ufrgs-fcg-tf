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
            GroundEnemyController(WalkerController* walker, engine::CapsuleCollider* capsule_collider) : walker(walker), capsule_collider(capsule_collider) {}

            void Awake() override;
            void Update() override;

            engine::CapsuleCollider* get_capsule_collider() { return this->capsule_collider; }

        private:
            // Walker component used to perform physics for this enemy
            WalkerController* walker = nullptr;
            engine::CapsuleCollider* capsule_collider = nullptr;

            // Target references
            PlayerController* target_controller = nullptr;
            engine::VObject* target_vobj = nullptr;

            float facing_smooth = 0.02f;
            // Internal helpers
            void update_follow_target();
    };
} // namespace game::components
