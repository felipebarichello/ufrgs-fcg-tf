#pragma once

#include <engine>
#include <memory>
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>
#include <game/components/PlanetInfo.hpp>
#include <game/components/player/SphericalInput.hpp>
#include <game/components/physics/KinematicBody.hpp>
#include <game/components/physics/AngularVelocity.hpp>
#include <game/components/SpaceshipController.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    class PlayerShipController : public engine::Behavior {
        public:
            PlayerShipController(SpaceshipController* ship_controller, engine::ObjDrawable* model, engine::CylinderCollider* cylinder_collider) : ship_controller(ship_controller), planets(scenes::main_scene::planets), cylinder_collider(cylinder_collider), model(model) {}
            void Awake() override;
            void Start() override;
            void Update() override;
            void PostUpdate() override;
            void OnEnable() override;
            void OnDisable() override;

            KinematicBody* get_kinematic() const { return this->kinematic; }
            AngularVelocity* get_angular() const { return this->angular; }
            float get_critical_roll_velocity() const { return this->ship_controller->get_critical_roll_velocity(); }

        private:
            SpaceshipController* ship_controller;
            KinematicBody* kinematic;
            AngularVelocity* angular;
            ShipCommand* ship_command;

            std::vector<PlanetInfo*> planets;
            engine::CylinderCollider* cylinder_collider;

            SphericalInput smooth_spherical_input;
            float theta_lerp = 0.2f;
            float phi_lerp = 0.2f;

            engine::ObjDrawable* model;
            
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;

            // Text drawable to show fuel on screen
            engine::TextDrawable* fuel_text = nullptr;
            engine::TextDrawable* game_over_text = nullptr;

            SphericalInput get_smooth_spherical_input();
            SphericalInput get_spherical_input();
            void test_planet_collisions();
    };
} // namespace game::components
