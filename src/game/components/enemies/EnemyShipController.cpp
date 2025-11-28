#include "EnemyShipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <engine/vobject/components/Drawable.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
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

        // Attach engine-level debug drawable to draw a line to the player
        if (this->debug_drawable == nullptr) {
            auto getter1 = [this]() -> engine::Vec3 {
                return this->debug_line_start;
            };
            auto getter2 = [this]() -> engine::Vec3 {
                return this->debug_line_end;
            };

            auto dbg = new engine::DebugLineDrawable(getter1, getter2, engine::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
            this->debug_drawable = dbg;
            this->get_vobject()->add_component(dbg);
        }
    }

    void EnemyShipController::Update() {
        // Simple AI to follow the player ship

        float dt = EngineController::get_delta_time();
        Transform& transform = this->get_vobject()->transform();
        KinematicBody& kinematic = *this->kinematic;
        AngularVelocity& angular = *this->angular;

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
        // nerror: negative error; add this to correct

        Vec3 pred_pl_pos = player_pos + player_vel * this->prediction_time;
        Vec3 pred_en_pos = enemy_pos  + enemy_vel  * this->prediction_time;

        Vec3 pred_vec_to_pl = pred_pl_pos - pred_en_pos;
        float pred_dist_to_pl = engine::norm(pred_vec_to_pl);
        if (pred_dist_to_pl < 1e-6f) {
            this->player_controller->hit_by_enemy(); // Prevent division by zero by just hitting the player
            return;
        }

        Vec3 pred_dir_to_pl = pred_vec_to_pl / pred_dist_to_pl;
        
        Vec3 cur_forward_dir = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        Quaternion cur_adjust_quat = Quaternion::from_unit_vectors(cur_forward_dir, pred_dir_to_pl);
        Quaternion cur_adjust_local = transform.quaternion().conjugated() * cur_adjust_quat * transform.quaternion();
        Vec3 cur_adjust_euler = cur_adjust_local.to_euler_zyx();

        Vec3 pred_en_euler = angular.euler_angles() * this->prediction_time;
        Vec3 euler_nerror = cur_adjust_euler - pred_en_euler;

        this->ship_command->steer = SphericalCoords {
            .delta_theta = euler_nerror.y * dt,
            .delta_phi   = euler_nerror.x * dt
        };
        
        float desired_roll_amount = -cur_adjust_euler.z;
        float cur_roll_rate = -angular.euler_angles().z;
        float pred_roll_amount = cur_roll_rate * this->prediction_time;
        float roll_amount_nerror = desired_roll_amount - pred_roll_amount;
        
        if (roll_amount_nerror > this->roll_error_tolerance) {
            this->ship_command->rolling_left  = false;
            this->ship_command->rolling_right = true;
        } else if (roll_amount_nerror < -this->roll_error_tolerance) {
            this->ship_command->rolling_left  = true;
            this->ship_command->rolling_right = false;
        } else {
            this->ship_command->rolling_left  = false;
            this->ship_command->rolling_right = false;
        }
        
        // Thrust if facing roughly towards the player's predicted position
        float pred_dir_to_pl_proj_cur_forward_dist = engine::dot(pred_dir_to_pl, cur_forward_dir);
        this->ship_command->thrusting = (pred_dir_to_pl_proj_cur_forward_dist > this->facing_player_threshold);
    }

    void EnemyShipController::PostUpdate() {
        this->test_planet_collisions();
    }

    void EnemyShipController::test_planet_collisions() {
        for (PlanetInfo* planet : this->planets) {
            bool collision_detected = engine::collision::collide_cylinder_sphere(*this->cylinder_collider, *planet->get_sphere_collider()).has_collided();
            if (collision_detected) {
                // this->get_vobject()->destroy();
            }
        }
    }
}