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

            KinematicBody* get_kinematic() const { return this->kinematic; }
            AngularVelocity* get_angular() const { return this->angular; }
            float get_critical_roll_velocity() const { return this->ship_controller->critical_roll_velocity; }

        private:
            SpaceshipController* ship_controller;
            KinematicBody* kinematic;
            AngularVelocity* angular;
            ShipCommand* ship_command;
            PlayerController* player_controller;

            std::vector<PlanetInfo*> planets;
            engine::CylinderCollider* cylinder_collider;

            void test_planet_collisions();
    };
} // namespace game::components
