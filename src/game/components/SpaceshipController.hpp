#pragma once

#include <engine>
#include <memory>
// Text rendering helper
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>
#include <game/components/PlanetInfo.hpp>

using engine::Transform;

namespace game::components {
    class SpaceshipController : public engine::Behavior {
        public:
            SpaceshipController(std::vector<PlanetInfo*> planets, engine::ObjDrawable* model, engine::CylinderCollider* cylinder_collider) : planets(planets), cylinder_collider(cylinder_collider), model(model) {}
            void Start() override;
            void Update() override;
            engine::Vec3 get_velocity() const { return this->velocity; }

            void enable();
            void disable();
            bool is_active() const { return this->active; }

            float speed = 10.0f;

        private:
            std::vector<PlanetInfo*> planets;
            engine::CylinderCollider* cylinder_collider;
            struct SphericalInput;

            engine::ObjDrawable* model;
            
            // Camera smoothing parameters
            engine::Vec2 move_vector {0.0f, 0.0f};
            engine::Vec3 current_velocity {0.0f, 0.0f, 0.0f};
            float v_sensitivity = 0.001f;
            float h_sensitivity = 0.001f;
            float thrust_power = 50.0f; // meters/s^2 applied when thrusting
            float fuel = 100.0f;
            float fuel_consumption_rate = 1.0f; // units per second when thrusting

            // Text drawable to show fuel on screen
            TextDrawable* fuel_text = nullptr;
            TextDrawable* game_over_text = nullptr;

            void update_rotation_due_to_input();
            engine::Vec3 velocity = engine::Vec3(0.0f);
            bool accelerating_forward = false;
            bool accelerating_backward = false;
            bool active = true;
            SphericalInput get_spherical_input();
            void test_planet_collisions();
    };
} // namespace game::components
