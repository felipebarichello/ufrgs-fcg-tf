#include "SpaceshipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace engine;

namespace game::components {

    void SpaceshipController::Update() {
        if (this->fuel > 0.0f) {
            float dt = EngineController::get_delta_time();
            Transform& transform = this->get_vobject()->transform();
            Vec3 forward = transform.get_quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));

            ShipCommand& cmd = this->command;

            // Passive fuel consumption
            this->fuel -= this->passive_fuel_consumption * dt;

            // Thrusting
            if (cmd.thrusting) {
                this->fuel -= dt * this->thrust_fuel_consumption;
                this->kinematic->set_velocity(this->kinematic->get_velocity() + forward * thrust_power * dt);
            }

            // Rolling
            Vec3& ang_velocity = this->angular->euler_angles();
            float& roll_velocity = ang_velocity.z;
            if (cmd.rolling_left) {
                this->fuel -= dt * this->roll_fuel_consumption;
                roll_velocity -= roll_power * dt;

            }
            if (cmd.rolling_right) {
                this->fuel -= dt * this->roll_fuel_consumption;
                roll_velocity += roll_power * dt;
            }
            if (cmd.rolling_left == false && cmd.rolling_right == false) {
                // Automatic ship stabilization when not actively rolling
                if (roll_velocity > 1e-6f) {
                    float desired_correction = -roll_velocity * this->auto_unroll_factor;
                    float abs_correction = std::fabs(desired_correction);
                    float auto_roll_power = std::min(abs_correction, roll_power);
                    float ratio = auto_roll_power / abs_correction;

                    this->fuel -= dt * (this->roll_fuel_consumption * ratio);
                    roll_velocity += std::copysignf(auto_roll_power, desired_correction) * dt;
                } else if (roll_velocity < -1e-6f) {
                    roll_velocity += roll_power * dt;
                    if (roll_velocity > 0.0f) {
                        roll_velocity = 0.0f;
                    }
                }
            }

            // Steering
            SphericalInput spherical = this->command.steer;
            float horizontal_fuel = std::fabs(spherical.delta_theta) * this->horizontal_steer_fuel_consumption;
            float vertical_fuel = std::fabs(spherical.delta_phi) * this->vertical_steer_fuel_consumption;
            this->fuel -= horizontal_fuel + vertical_fuel;
            this->angular->euler_angles().y = spherical.delta_theta * this->horizontal_steer_power;
            this->angular->euler_angles().x = spherical.delta_phi * this->vertical_steer_power;
        } else {
            this->fuel = 0.0f; // Prevent negative fuel
        }
    }

}