#pragma once

#include <engine>
#include <engine/collision/colliders/PointCollider.hpp>
#include <memory>
#include <vector>
#include <optional>
#include <game/components/PlanetInfo.hpp>

namespace game::components {
    class WalkerController : public engine::Behavior {
        public:
            WalkerController(std::vector<PlanetInfo*> planets, engine::PointCollider* point_collider, engine::CylinderCollider* cylinder_collider) : planets(planets), point_collider(point_collider), cylinder_collider(cylinder_collider) {}
            void Update() override;

            // Input interface (called by HumanoidPlayerController)
            void set_move_vector(engine::Vec2 mv) { this->move_vector = mv; }
            // Query input state for other components (e.g. camera bobbing)
            engine::Vec2 get_move_vector() const { return this->move_vector; }
            void request_jump() { this->jump_requested = true; }
            engine::PointCollider* get_point_collider() { return this->point_collider; }
            engine::CylinderCollider* get_cylinder_collider() { return this->cylinder_collider; }

        private:
            engine::Vec2 move_vector {0.0f, 0.0f};
            bool jump_requested = false;

            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};

            // Tuning
            float walk_accel = 70.0f;
            float walk_deaccel = 200.0f;
            float max_walk_speed = 30.0f;
            float jump_strength = 80.0f;

            // Gravity / grounding
            static constexpr float GRAVITATIONAL_CONSTANT = 6.6743e-11f;
            std::vector<PlanetInfo*> planets;
            std::optional<PlanetInfo*> grounded_to = std::nullopt;
            PlanetInfo* closest_planet = nullptr;

            // Point collider attached to this walker (created in Start)
            engine::PointCollider* point_collider;
            engine::CylinderCollider* cylinder_collider;

            void update_transform_due_to_environment();
            void update_transform_due_to_input();
            engine::Vec3 compute_equivalent_gravity();
            void correct_planet_collision();
            void align_to_closest_planet();
    };
} // namespace game::components
