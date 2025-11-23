#pragma once

#include <engine>
#include <memory>
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>
#include <game/components/PlanetInfo.hpp>
#include <game/components/player/SphericalInput.hpp>
#include <game/components/KinematicBody.hpp>
#include <game/components/AngularVelocity.hpp>
#include <game/components/SpaceshipController.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(KinematicBody* kinematic, AngularVelocity* angular): kinematic(kinematic), angular(angular) {}
            // void Awake() override;
            // void Start() override;
            // void Update() override;
            // void PostUpdate() override;
            // void OnEnable() override;
            // void OnDisable() override;

            KinematicBody* get_kinematic_body() const { return this->kinematic; }
            AngularVelocity* get_angular_velocity() const { return this->angular; }
            // float get_critical_roll_velocity() const { return this->critical_roll_velocity; }

            float speed = 10.0f;

        private:
            KinematicBody* kinematic;
            AngularVelocity* angular;
            // std::vector<PlanetInfo*> planets;
            // engine::CylinderCollider* cylinder_collider;

            // SphericalInput smooth_spherical_input;
            // float theta_lerp = 0.2f;
            // float phi_lerp = 0.2f;

            // engine::ObjDrawable* model;
            
            // float v_sensitivity = 0.001f;
            // float h_sensitivity = 0.001f;
            // float thrust_power = 50.0f; // meters/s^2 applied when thrusting
            // float roll_power = 8.0f; // radians/s^2 applied when rolling
            // float auto_unroll_factor = 1.0f; // Ratio of current angular velocity corrected per second when not rolling
            // float critical_roll_velocity = 10.0f; // radians/s before ship is destroyed
            // float vertical_steer_power = 50.0f; // radians/s applied per unit of vertical input
            // float horizontal_steer_power = 50.0f; // radians/s applied per unit of horizontal input

            // float fuel = 200.0f;
            // float thrust_fuel_consumption = 10.0f; // units per second when thrusting
            // float roll_fuel_consumption = 3.0f; // units per second when rolling
            // float vertical_steer_fuel_consumption = 2.0f; // units per second per unit of vertical input
            // float horizontal_steer_fuel_consumption = 2.0f; // units per second per unit of horizontal input
            // float passive_fuel_consumption = 1.0f; // units per second whenever
            
            // bool thrusting = false;
            // bool rolling_left = false;
            // bool rolling_right = false;

            // // Text drawable to show fuel on screen
            // engine::TextDrawable* fuel_text = nullptr;
            // engine::TextDrawable* game_over_text = nullptr;

            // SphericalInput get_smooth_spherical_input();
            // SphericalInput get_spherical_input();
            // void test_planet_collisions();
    };
} // namespace game::components
