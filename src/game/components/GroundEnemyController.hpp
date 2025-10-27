#pragma once

#include <engine>
#include <memory>
#include <vector>
#include <optional>
#include <game/components/player/HumanoidPlayerController.hpp>

namespace game::components {
    class GroundEnemyController : public engine::Behavior {
        public:
            /// @param target The VObject the enemy should try to follow (usually the player VObject)
            /// @param planets List of planets for gravity/grounding calculations
            GroundEnemyController(HumanoidPlayerController* target_controller, std::vector<PlanetInfo*> planets) : target_controller(target_controller), planets(planets) {}

            void Awake() override;
            void Update() override;

        private:
            // Movement state
            HumanoidPlayerController* target_controller = nullptr;
            engine::VObject* target_vobj = nullptr;
            std::vector<PlanetInfo*> planets;
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};

            // Tuning
            float move_accel = 10.0f;         // acceleration towards the target
            float move_deaccel = 120.0f;       // deceleration on surface
            float max_move_speed = 40.0f;     // max horizontal speed
            float jump_strength = 50.0f;      // TODO: Use

            // Gravity / grounding
            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            std::optional<PlanetInfo*> grounded_to = std::nullopt;
            PlanetInfo* closest_planet = nullptr;

            // Internal helpers
            void update_transform_due_to_environment();
            void update_follow_target();
            engine::Vec3 compute_equivalent_gravity();
            void correct_planet_collision();
            void align_to_closest_planet();
    };
} // namespace game::components
