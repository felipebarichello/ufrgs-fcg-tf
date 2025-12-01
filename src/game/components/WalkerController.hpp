#pragma once

#include <engine>
#include <engine/collision/colliders/PointCollider.hpp>
#include <memory>
#include <vector>
#include <optional>
#include <game/components/PlanetInfo.hpp>
#include <game/components/physics/KinematicBody.hpp>
#include <game/components/physics/Gravity.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    class WalkerController : public engine::Behavior {
        public:
            WalkerController(KinematicBody* kinematic, Gravity* gravity, engine::PointCollider* point_collider) : kinematic(kinematic), gravity(gravity), planets(scenes::main_scene::planets), point_collider(point_collider) {}
            void Update() override;
            void PostUpdate() override;

            // Input interface (called by HumanoidPlayerController)
            void set_move_vector(engine::Vec2 mv) { this->move_vector = mv; }
            // Query input state for other components (e.g. camera bobbing)
            engine::Vec2 get_move_vector() const { return this->move_vector; }
            void request_jump();
            engine::PointCollider* get_point_collider() { return this->point_collider; }
            engine::Vec3 get_velocity() const { return this->kinematic->get_velocity(); }
            bool is_grounded() const { return this->grounded_to.has_value(); }
            void set_max_walk_speed(float speed) { this->max_walk_speed = speed; }

        private:
            KinematicBody* kinematic;
            Gravity* gravity;

            engine::Vec2 move_vector {0.0f, 0.0f};

            // Tuning
            float walk_accel = 70.0f;
            float walk_deaccel = 200.0f;
            float max_walk_speed = 30.0f;
            float jump_strength = 20.0f;

            std::vector<PlanetInfo*> planets;
            std::optional<PlanetInfo*> grounded_to = std::nullopt;
            PlanetInfo* closest_planet = nullptr;

            engine::PointCollider* point_collider;

            void update_transform_due_to_input();
            void correct_planet_collision();
            void align_to_closest_planet();
            void ground_to(PlanetInfo* planet);
            void set_not_grounded();
    };
} // namespace game::components
