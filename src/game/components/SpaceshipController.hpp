#pragma once

#include <engine>
#include <memory>
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>
#include <game/components/PlanetInfo.hpp>
#include <game/components/SphericalInput.hpp>
#include <game/components/KinematicBody.hpp>
#include <game/scenes/MainScene_vars.hpp>

namespace game::components {
    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(KinematicBody* kinematic, engine::ObjDrawable* model, engine::CylinderCollider* cylinder_collider) : kinematic(kinematic), planets(scenes::main_scene::planets), cylinder_collider(cylinder_collider), model(model) {}
            void Start() override;
            void Update() override;
            void PostUpdate() override;
            void OnEnable() override;
            void OnDisable() override;

            float speed = 10.0f;

        private:
            KinematicBody* kinematic;
            std::vector<PlanetInfo*> planets;
            engine::CylinderCollider* cylinder_collider;

            engine::ObjDrawable* model;
            
            // Camera smoothing parameters
            engine::Vec2 move_vector {0.0f, 0.0f};
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            float thrust_power = 50.0f; // meters/s^2 applied when thrusting
            float fuel = 100.0f;
            float thrust_fuel_consumption = 1.0f; // units per second when thrusting
            float passive_fuel_consumption = 0.1f; // units per second whenever

            // Text drawable to show fuel on screen
            engine::TextDrawable* fuel_text = nullptr;
            engine::TextDrawable* game_over_text = nullptr;

            void update_rotation_due_to_input();
            bool accelerating_forward = false;
            bool accelerating_backward = false;
            SphericalInput get_spherical_input();
            void test_planet_collisions();
    };
} // namespace game::components
