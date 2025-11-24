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

        Transform& transform = this->get_vobject()->transform();
        KinematicBody& kinematic = *this->kinematic;

        // Get positions
        Vec3 enemy_pos = transform.get_world_position();
        Vec3 enemy_vel = kinematic.get_velocity();
        PlayerShipController* player_ship = this->player_controller->get_ship();
        Vec3 player_pos = player_ship->get_vobject()->transform().get_world_position();
        Vec3 player_vel = player_ship->get_kinematic_body()->get_velocity();

        // Abbreviations:
        // pred: predicted
        // cur: current
        // dist: distance
        // vec: vector
        // dir: direction
        // proj: projected onto
        // pl: player
        // en: enemy

        Vec3 pred_pl_pos = player_pos + player_vel * this->prediction_time;
        Vec3 pred_en_pos = enemy_pos  + enemy_vel  * this->prediction_time;

        Vec3 pred_vec_to_pl = pred_pl_pos - pred_en_pos;
        float pred_dist_to_pl = engine::norm(pred_vec_to_pl);
        if (pred_dist_to_pl < 1e-6f) {
            this->player_controller->hit_by_enemy(); // Prevent division by zero by just hitting the player
            return;
        }

        Vec3 cur_forward_dir = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        float pred_vec_to_pl_proj_cur_forward_dist = engine::dot(pred_vec_to_pl, cur_forward_dir);
        Vec3 pred_dir_to_pl = pred_vec_to_pl / pred_dist_to_pl;

        Quaternion pred_quat_adjust = Quaternion::from_unit_vectors(cur_forward_dir, pred_dir_to_pl);
        Vec3 pred_euler_adjust = pred_quat_adjust.to_euler_zyx();

        // Set steering input
        this->ship_command->steer = SphericalCoords {
            .delta_theta = pred_euler_adjust.y,
            .delta_phi   = pred_euler_adjust.x
        };

        float desired_roll = -pred_euler_adjust.z;
        float pred_avg_roll_rate = desired_roll / this->prediction_time;
        float cur_roll_rate = this->angular->euler_angles().z;
        float roll_rate_error = pred_avg_roll_rate - cur_roll_rate;
        
        if (roll_rate_error > this->roll_error_tolerance) {
            this->ship_command->rolling_left  = false;
            this->ship_command->rolling_right = true;
        } else if (roll_rate_error < -this->roll_error_tolerance) {
            this->ship_command->rolling_left  = true;
            this->ship_command->rolling_right = false;
        } else {
            this->ship_command->rolling_left  = false;
            this->ship_command->rolling_right = false;
        }

        // Thrust if not too close
        this->ship_command->thrusting = (pred_vec_to_pl_proj_cur_forward_dist > 0.0f);
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