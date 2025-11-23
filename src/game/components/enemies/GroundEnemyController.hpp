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
            GroundEnemyController(WalkerController* walker, engine::CylinderCollider* cylinder_collider) : walker(walker), cylinder_collider(cylinder_collider) {}

            void Awake() override;
            void Update() override;

            engine::CylinderCollider* get_cylinder_collider() { return this->cylinder_collider; }

        private:
            // Walker component used to perform physics for this enemy
            WalkerController* walker = nullptr;
            engine::CylinderCollider* cylinder_collider = nullptr;

            // Target references
            HumanoidPlayerController* target_controller = nullptr;
            engine::VObject* target_vobj = nullptr;

            // Internal helpers
            void update_follow_target();
    };
} // namespace game::components
