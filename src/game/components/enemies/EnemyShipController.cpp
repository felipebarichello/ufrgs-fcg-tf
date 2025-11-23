#include "EnemyShipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <game/scenes/MainScene_vars.hpp>
#include <game/components/player/PlayerController.hpp>

using namespace engine;

namespace game::components {

    void EnemyShipController::Awake() {
        this->player_controller = scenes::main_scene::player;
        this->ship_command = &this->ship_controller->get_command();

        // Physics setup
        this->kinematic = this->ship_controller->get_kinematic_body();
        this->angular = this->ship_controller->get_angular_velocity();
    }

    void EnemyShipController::Update() {
        // Simple AI to follow the player ship

        float dt = EngineController::get_delta_time();
        Transform& transform = this->get_vobject()->transform();

        // Get positions
        Vec3 enemy_pos = transform.get_world_position();
        Vec3 player_pos = this->player_controller->get_ship()->get_vobject()->transform().get_world_position();

        Vec3 to_player = player_pos - enemy_pos;
        float dist = engine::norm(to_player);
        if (dist < 1e-6f) {
            this->player_controller->hit_by_enemy(); // Prevent division by zero by just hitting the player
            return;
        }

        Vec3 dir_to_player = to_player / dist;

        Vec3 forward = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right = transform.quaternion().rotate(Vec3(1.0f, 0.0f, 0.0f));
        Vec3 up = transform.quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));

        float forward_comp = engine::dot_product(dir_to_player, forward);
        float right_comp = engine::dot_product(dir_to_player, right);
        float up_comp = engine::dot_product(dir_to_player, up);
        // float forward_dist = dist * forward_comp;
        // float right_dist = dist * right_comp;
        // float up_dist = dist * up_comp;

        // Set steering input
        this->ship_command->steer = SphericalCoords(up_comp * dt, right_comp * dt);

        // Thrust if not too close
        this->ship_command->thrusting = (forward_comp > 10.0f);
    }

    void EnemyShipController::PostUpdate() {
        this->test_planet_collisions();
    }

    void EnemyShipController::test_planet_collisions() {
        for (PlanetInfo* planet : this->planets) {
            bool collision_detected = engine::collision::collide_cylinder_sphere(*this->cylinder_collider, *planet->get_sphere_collider()).has_collided();
            if (collision_detected) {
                this->get_vobject()->destroy();
            }
        }
    }
}