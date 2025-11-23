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
    struct ShipCommand {
        bool thrusting = false;
        bool rolling_left = false;
        bool rolling_right = false;
        SphericalCoords steer;
    };

    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(KinematicBody* kinematic, AngularVelocity* angular): kinematic(kinematic), angular(angular) {}
            void Update() override;

            KinematicBody* get_kinematic_body() const { return this->kinematic; }
            AngularVelocity* get_angular_velocity() const { return this->angular; }
            ShipCommand& get_command() { return this->command; }
            float get_fuel() const { return this->fuel; }
            float get_critical_roll_velocity() const { return this->critical_roll_velocity; }

        private:
            KinematicBody* kinematic;
            AngularVelocity* angular;
            ShipCommand command;

            float thrust_power = 50.0f; // meters/s^2 applied when thrusting
            float roll_power = 8.0f; // radians/s^2 applied when rolling
            float auto_unroll_factor = 1.0f; // Ratio of current angular velocity corrected per second when not rolling
            float critical_roll_velocity = 10.0f; // radians/s before ship is destroyed
            float vertical_steer_power = 50.0f; // radians/s applied per unit of vertical input
            float horizontal_steer_power = 50.0f; // radians/s applied per unit of horizontal input

            float fuel = 200.0f;
            float thrust_fuel_consumption = 10.0f; // units per second when thrusting
            float roll_fuel_consumption = 3.0f; // units per second when rolling
            float vertical_steer_fuel_consumption = 2.0f; // units per second per unit of vertical input
            float horizontal_steer_fuel_consumption = 2.0f; // units per second per unit of horizontal input
            float passive_fuel_consumption = 1.0f; // units per second whenever
    };
} // namespace game::components
