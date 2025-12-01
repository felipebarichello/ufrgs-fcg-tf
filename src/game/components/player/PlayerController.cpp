#include "PlayerController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>
#include <sstream>
#include <iomanip>
#include <game/scenes/MainScene_vars.hpp>

using namespace engine;

namespace game::components {

    void PlayerController::Awake() {
        // Set main camera to humanoid camera at start
        Camera::set_main(this->humanoid_cam);

        /* HUD setup */

        this->game_over_text = new TextDrawable();
        this->game_over_text->setText(std::string(""), 1.8f, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
        scenes::main_scene::game_over_text = this->game_over_text;

        TextDrawable* f_text = new TextDrawable();
        TextDrawable* go_text = new TextDrawable();
        this->fuel_text = f_text;
        this->game_over_text = go_text;
        this->get_vobject()->add_component(f_text);
        this->get_vobject()->add_component(go_text);

        // Create text drawable to show fuel on-screen (top-left corner)
        std::ostringstream init_ss;
        init_ss << std::fixed << std::setprecision(1) << this->ship->get_ship_controller()->fuel;
        this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
        this->game_over_text->setText(std::string(""), 3.0f, engine::Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);

        // Create text drawable to show oxygen on-screen (top-left corner, below fuel)
        this->oxygen_text = new TextDrawable();
        this->get_vobject()->add_component(this->oxygen_text);
        std::ostringstream oxy_ss;;
        oxy_ss << std::fixed << std::setprecision(1) << this->oxygen_level;
        this->oxygen_text->setText(std::string("Oxygen: ") + oxy_ss.str(), 1.5f, engine::Vec3(0.0f, 0.7f, 1.0f), -0.95f, 0.8f);
    }

    void PlayerController::Start() {
        InputController* input = EngineController::get_input();
        
        // Toggle on 'E' press
        input->subscribe_press_button(GLFW_KEY_E, std::bind(&PlayerController::toggle_mode, this));

        this->ship->disable();
    }

    void PlayerController::Update() {
        // Destroy ship if rolling too fast
        if (this->ship->is_enabled() && std::fabs(this->ship->get_angular_velocity()->euler_angles().z) > this->ship->get_critical_roll_velocity()) {
            this->toggle_mode();
        }
    }

    void PlayerController::PostUpdate() {

        float delta_time = EngineController::get_delta_time();
        // Decrease oxygen level over time if it is humanoid and is not grouded or is in the ship but there is no fuel

        bool is_humanoid_active = this->humanoid && this->humanoid->is_enabled();
        bool is_ship_active = this->ship && this->ship->is_enabled();
        if ((is_humanoid_active && !this->humanoid->get_walker()->is_grounded()) || (is_ship_active && this->ship->get_ship_controller()->fuel <= 0.0f)) {
            this->oxygen_level -= this->oxygen_decrease_rate * delta_time;
            if (this->oxygen_level < 0.0f) {
                this->oxygen_level = 0.0f;
                // Game over
                this->hit_by_enemy();
            }
        } else {
            // Recover oxygen when grounded
            this->oxygen_level += (this->oxygen_decrease_rate / 2.0f) * delta_time;
            if (this->oxygen_level > 100.0f) {
                this->oxygen_level = 100.0f;
            }
        }

        // if is grounded add fuel to the ship
        if (is_humanoid_active && this->humanoid->get_walker()->is_grounded()) {
            this->ship->get_ship_controller()->add_fuel(this->fuel_increase_rate * delta_time);
        }

        // Update HUD fuel text
        if (this->fuel_text) {
            std::ostringstream init_ss;
            init_ss << std::fixed << std::setprecision(1) << this->ship->get_ship_controller()->fuel;
            this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
        }
        // Update HUD oxygen text
        if (this->oxygen_text && this->humanoid) {
            std::ostringstream oxy_ss;;
            oxy_ss << std::fixed << std::setprecision(1) << this->oxygen_level;
            this->oxygen_text->setText(std::string("Oxygen: ") + oxy_ss.str(), 1.5f, engine::Vec3(0.0f, 0.7f, 1.0f), -0.95f, 0.8f);
        }
    }

    void PlayerController::hit_by_enemy() {
        this->game_over_text->setText(std::string("GAME OVER"), 1.8f, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
    }

    void PlayerController::toggle_mode() {
        if (!this->humanoid || !this->ship) return;
        if (this->humanoid->get_walker()->is_grounded()) return; // Only allow switching when humanoid is not grounded

        bool was_humanoid = this->humanoid->is_enabled();
        if (was_humanoid) {
            // Can't switch to ship if out of fuel
            if (this->ship->get_ship_controller()->fuel <= 0.0f) {
                return;
            }

            float phi = this->humanoid->get_camera_phi();
            Quaternion rotation_adjustment = Quaternion::from_x_rotation(phi);
            this->get_vobject()->transform().local_quaternion().local_compose(rotation_adjustment);
            this->humanoid->reset_camera_phi();
            
            this->humanoid->disable();
            this->ship->enable();
            Camera::set_main(this->ship_cam);
        } else {
            this->ship->disable();
            this->humanoid->enable();
            Camera::set_main(this->humanoid_cam);
        }
    }

}
