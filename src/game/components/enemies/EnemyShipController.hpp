#pragma once

#include <engine>
#include <memory>
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>
#include <game/components/PlanetInfo.hpp>
#include <game/components/SphericalCoords.hpp>
#include <game/components/physics/KinematicBody.hpp>
#include <game/components/physics/AngularVelocity.hpp>
#include <game/components/SpaceshipController.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    class EnemyShipController : public engine::Behavior {
        public:
            EnemyShipController(SpaceshipController* ship_controller, engine::CylinderCollider* cylinder_collider) : ship_controller(ship_controller), planets(scenes::main_scene::planets), cylinder_collider(cylinder_collider) {}
            void Awake() override;
            void Update() override;
            void PostUpdate() override;

            KinematicBody* get_kinematic_body() const { return this->kinematic; }
            AngularVelocity* get_angular_velocity() const { return this->angular; }
            float get_critical_roll_velocity() const { return this->ship_controller->critical_roll_velocity; }

        private:
            SpaceshipController* ship_controller;
            KinematicBody* kinematic;
            AngularVelocity* angular;
            ShipCommand* ship_command;
            PlayerController* player_controller;

            std::vector<PlanetInfo*> planets;
            engine::CylinderCollider* cylinder_collider;

            float catch_speed = 0.5f; // Desired meters per second when touching player
            float max_steer_velocity = 4.0f; // Radians per second of max steering input
            float facing_player_threshold = 0.8f; // Dot product within which to consider "facing player" for thrusting
            float prediction_time = 1.0f; // Seconds to look ahead for player and enemy state
            float roll_error_tolerance = 0.1f; // Predicted radians of error allowed before rolling to correct

            void test_planet_collisions();

            // Debug line
            engine::DebugLineDrawable* debug_drawable = nullptr;
            engine::Vec3 debug_line_start = engine::Vec3(0.0f);
            engine::Vec3 debug_line_end   = engine::Vec3(0.0f);
    };
} // namespace game::components
