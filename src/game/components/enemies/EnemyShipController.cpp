#include "EnemyShipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <game/scenes/MainScene_vars.hpp>
#include <game/components/player/PlayerController.hpp>
#include <macros/include.hpp>

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

        // Vec3 pred_pl_pos = player_pos + player_vel * this->prediction_time;
        // Vec3 pred_en_pos = enemy_pos  + enemy_vel  * this->prediction_time;

        // Vec3 pred_vec_to_pl = pred_pl_pos - pred_en_pos;
        // float pred_dist_to_pl = engine::norm(pred_vec_to_pl);
        // if (pred_dist_to_pl < 1e-6f) {
        //     this->player_controller->hit_by_enemy(); // Prevent division by zero by just hitting the player
        //     return;
        // }

        // Vec3 pred_dir_to_pl = pred_vec_to_pl / pred_dist_to_pl;
        
        // Vec3 cur_forward_dir = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        // Quaternion cur_adjust_quat = Quaternion::from_unit_vectors(cur_forward_dir, pred_dir_to_pl);
        // Quaternion cur_adjust_local = transform.quaternion().conjugated() * cur_adjust_quat * transform.quaternion();
        // Vec3 cur_adjust_euler = cur_adjust_local.to_euler_zyx();

        // Vec3 pred_en_euler = angular.euler_angles() * this->prediction_time;
        // Vec3 euler_nerror = cur_adjust_euler - pred_en_euler;
        // float theta_vel = copysignf(std::min(std::fabs(euler_nerror.y), this->max_steer_velocity * this->prediction_time), euler_nerror.y);
        // float phi_vel   = copysignf(std::min(std::fabs(euler_nerror.x), this->max_steer_velocity * this->prediction_time), euler_nerror.x);

        Vec3 vec_to_player = player_pos - enemy_pos;
        float player_distance = engine::norm(vec_to_player);
        if (player_distance < 1e-6f) {
            // Should never happen, but just in case
            DEBUG_LOG("WARNING: Enemy ship and player positions are identical!");
            return;
        }

        Vec3 player_direction = vec_to_player / player_distance;
        Vec3 rel_player_vel = player_vel - enemy_vel; // Relative velocity of player with respect to enemy
        float closing_speed = engine::dot(rel_player_vel, player_direction);
        Vec3 closing_velocity = closing_speed * player_direction;
        Vec3 tangential_velocity = rel_player_vel - closing_velocity;
        float tangential_speed = engine::norm(tangential_velocity);

        // The enemy AI currently has three main objectives:
        // - Maximize closing speed
        // - Minimize overshoot
        // - Minimize tangential speed (to avoid circling around the player)

        /* Calculate time to catch up to the player in perfect conditions */

        // v(t_catch) = catch_speed
        // v(t) = v(0) + INTEGRAL OF a(t) dt FROM 0 TO t
        // v(t_catch) = v(0) + INTEGRAL OF a(t) dt FROM 0 TO t_catch
        // INTEGRAL OF a(t) dt FROM 0 TO t_catch = catch_speed - v(0)
        // INTEGRAL OF a(t) dt FROM 0 TO t_catch = closing_speed_adjustment
        float closing_speed_adjustment = this->catch_speed - closing_speed;

        // s(t) = s(0) + INTEGRAL OF v(t) dt FROM 0 TO t
        // s(t_catch) = s(0) + INTEGRAL OF v(t) dt FROM 0 TO t_catch
        // s(t_catch) = 0
        // -s(0) = INTEGRAL OF    [v(0) + INTEGRAL OF a(t) dt FROM 0 TO t] dt    FROM 0 TO t_catch
        // -s(0) =
        //       INTEGRAL OF v(0) dt FROM 0 TO t_catch
        //     + INTEGRAL OF  [INTEGRAL OF a(t) dt FROM 0 TO t] dt  FROM 0 TO t_catch
        // 
        // There is no reason not to set acceleration as high as possible at all times,
        // so we can assume a(t) = a_max * a_dir(t)
        // where a_dir(t) is a function that is 1 when we want to speed up and -1 when we want to slow down.
        // Thus:
        // -s(0) =
        //       t_catch * v(0)
        //     + INTEGRAL FROM 0 TO t_catch OF [
        //           INTEGRAL OF  a_max a_dir(t) dt  FROM 0 TO t
        //       ] dt
        // [(a_dir(t) + 1) / 2] is 1 when we want to speed up and 0 when we want to slow down.
        // Let's call that function a_dir_bin(t)
        // a_dir(t) = a_dir_bin(t) * 2 - 1
        // Later we'll need to split the integral into two parts to solve for t_catch.
        // So we also need to know if a_dir(t) starts from 1 or -1.
        // Therefore, let's force it to start from 1 (speeding up), and call that function a_dir_fwd(t).
        // a_dir_fwd(t) = starting_sign * a_dir(t)
        //     where is starting_sign = 1 if a_dir(t) starts from 1, -1 otherwise.
        //
        // Now let's define the binary version of a_dir_fwd(t) as a_dir_bin_fwd(t):
        // a_dir_fwd(t) = a_dir_bin_fwd(t) * 2 - 1
        // And now, for replacing it in the integral, we need a_dir(t) in terms of a_dir_bin_fwd(t):
        // starting_sign * a_dir(t) = a_dir_bin_fwd(t) * 2 - 1
        // a_dir(t) = [a_dir_bin_fwd(t) * 2 - 1] / starting_sign
        // a_dir(t) = [a_dir_bin_fwd(t) * 2 - 1] * starting_sign    # dividing and multiplying by starting_sign is the same operation
        //
        // Replacing a_dir(t) in the integral:
        // -s(0) =
        //       t_catch * v(0)
        //     + INTEGRAL FROM 0 TO t_catch OF [
        //           INTEGRAL OF    a_max [a_dir_bin_fwd(t) * 2 - 1] * starting_sign dt    FROM 0 TO t
        //       ] dt
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * INTEGRAL FROM 0 TO t_catch OF [
        //           INTEGRAL OF    2 a_dir_bin_fwd(t) dt    FROM 0 to t
        //         - INTEGRAL OF    1                  dt    FROM 0 TO t
        //       ] dt
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * INTEGRAL FROM 0 TO t_catch OF [
        //           2 * INTEGRAL OF    a_dir_bin_fwd(t) dt    FROM 0 to t
        //         - t
        //       ] dt
        //
        // Since a_dir_bin_fwd(t) is a binary function that starts from 1 and then switches to 0 exactly once,
        // we can define t_switch as the time when it switches from 1 to 0 and split the integral there:
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * INTEGRAL FROM 0 TO t_catch OF [
        //           -t + 2 * [
        //               INTEGRAL OF    1 dt    FROM 0 to t_switch
        //             + INTEGRAL OF    0 dt    FROM t_switch to t
        //           ]
        //       ] dt
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * INTEGRAL FROM 0 TO t_catch OF [
        //           -t + 2 * [t_switch - 0]
        //       ] dt
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * [
        //         - INTEGRAL FROM 0 TO t_catch OF  t          dt
        //         + INTEGRAL FROM 0 TO t_catch OF  2 t_switch dt
        //       ]
        // -s(0) =
        //       t_catch * v(0)
        //     + a_max * starting_sign * [
        //         - t_catch^2 / 2
        //         + 2 * t_switch * t_catch
        //       ]
        // -s(0) = t_catch * [
        //       v(0)
        //     + a_max * starting_sign * [
        //         - t_catch / 2
        //         + 2 * t_switch
        //       ]
        // ]
        // 
        // Now we can isolate t_catch:
        // t_catch = -s(0) / [
        //       v(0)
        //     + a_max * starting_sign * [
        //         - t_catch / 2
        //         + 2 * t_switch
        //       ]
        // ]
        // t_catch = -s(0) / [
        //       v(0)
        //     - a_max * starting_sign / 2 * t_catch
        //     + a_max * starting_sign * 2 * t_switch
        // ]
        // -s(0) = t_catch * [
        //       v(0)
        //     - a_max * starting_sign / 2 * t_catch
        //     + a_max * starting_sign * 2 * t_switch
        // ]
        //
        // For conciseness,
        //     samax = signed_a_max = a_max * starting_sign
        //     tc = t_catch
        //     ts = t_switch
        //     s0 = s(0)
        //     v0 = v(0)
        //
        // -s0 = tc * [
        //       v0
        //     - samax / 2 * tc
        //     + samax * 2 * ts
        // ]
        // 0 =
        //     - samax / 2             * tc^2
        //     + [v0 + 2 * samax * ts] * tc
        //     + s0
        // Solving the quadratic equation for tc:
        // 2a = -samax
        // -b = -v0 - 2 * samax * ts
        // b^2 - 4ac = (v0 + 2 * samax * ts)^2 - 4 * (-samax / 2) * s0
        // b^2 - 4ac = (v0 + 2 * samax * ts)^2 + 2 * samax * s0
        // tc = tcl + tcr
        // tcl = v0 / samax + 2 * ts
        // tcr = - sqrt((v0 + 2 * samax * ts)^2 + 2 * samax * s0) / samax
        // tc = [v0 - sqrt((v0 + 2 * samax * ts)^2 + 2 * samax * s0)] / samax + 2 * ts
        // 
        // Now everything is known except for tc itself and ts (starting_sign can be split into two cases later).
        // Since the enemy wants to minimize tc, we can should get the minimum of the function tc(ts) for ts in [0, tc].
        // Find critical points:
        // d/d(ts)  [v0 - sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)]  / samax    + 2 * ts    = 0
        // {d/d(ts) [v0 - sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)]} / samax    + 2         = 0
        // d/d(ts)  [v0 - sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)]           = -2 * samax
        // {d/d(ts) v0} - {d/d(ts) sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)}  = -2 * samax
        // 0            - {d/d(ts) sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)}  = -2 * samax
        // dsqrt * {d/d(ts) [v0 + 2 * samax * ts]^2  + 2 * samax * s0}  =  -2 * samax
        //     where dsqrt = derivative_of_sqrt = 1 / 2 sqrt([v0 + 2 * samax * ts]^2  + 2 * samax * s0)
        // {d/d(ts) [v0 + 2 * samax * ts]^2}                         +  {d/d(ts) 2 * samax * s0}  = -2 * samax / dsqrt
        // 2 [v0 + 2 * samax * ts] * {d/d(ts) v0 + 2 * samax * ts}   +  0                         = -2 * samax / dsqrt
        //                            0          + 2 * samax       = -2 * samax / dsqrt - 2 [v0 + 2 * samax * ts]
        // 2 * samax / dsqrt + 2 [v0 + 2 * samax * ts] + 2 * samax = 0
        // 1 / dsqrt + v0 / samax + 2 ts + 1 = 0
        // 1 / dsqrt + 2 ts + v0 / samax + 1 = 0
        // 1 / dsqrt + 2 ts + v0 / samax + 1 = 0
        // 2 sqrt([v0 + 2 * samax * ts]^2 + 2 * samax * s0) + 2 ts + v0 / samax + 1 = 0

        float v0 = closing_speed;
        float s0 = player_distance;
        float a_max = this->ship_controller->thrust_accel;
        float samax_pos = a_max;  // starting_sign = 1
        float samax_neg = -a_max; // starting_sign = -1

        // a_dir(t) can be any function that is positive when we want to speed up and negative when we want to slow down.
        // Deriving both sides of the equation:
        // a(t_catch) - a(0) = 0
        // a(t_catch) = [d/dt closing_speed_adjustment]

        Vec3 velocity_when_catch = this->catch_speed * player_direction;
        

        this->ship_command->steer = SphericalCoords {
            .delta_theta = theta_vel * dt,
            .delta_phi   = phi_vel   * dt
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
        
        // Thrust if currently facing roughly towards the predicted enemy-player direction
        // float pred_dir_to_pl_proj_pred_forward_dist = engine::dot(pred_dir_to_pl, cur_forward_dir);
        // this->ship_command->thrusting = (pred_dir_to_pl_proj_pred_forward_dist > this->facing_player_threshold);
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