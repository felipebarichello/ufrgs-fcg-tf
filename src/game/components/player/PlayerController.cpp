#include "PlayerController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>
#include <sstream>
#include <iomanip>
#include <game/scenes/MainScene_vars.hpp>
#include <game/scenes/MainScene.hpp>

using namespace engine;

namespace game::components {

    void PlayerController::Awake() {
        // Set main camera to humanoid camera at start
        Camera::set_main(this->humanoid_cam);

        /* HUD setup */

        this->game_over_text = new TextDrawable();
        this->game_over_text->setText(std::string(""), 1.8f, 0.0f, 0.0f);
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
        this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, -0.95f, 0.9f);
        this->game_over_text->setText(std::string(""), 3.0f, 0.0f, 0.0f);

        // Create text drawable to show oxygen on-screen (top-left corner, below fuel)
        this->oxygen_text = new TextDrawable();
        this->get_vobject()->add_component(this->oxygen_text);
        std::ostringstream oxy_ss;;
        oxy_ss << std::fixed << std::setprecision(1) << this->oxygen_level;
        this->oxygen_text->setText(std::string("Oxygen: ") + oxy_ss.str(), 1.5f, -0.95f, 0.8f);

        this->time_text = new TextDrawable();
        this->get_vobject()->add_component(this->time_text);
        this->time_text->setText("", 1.5f, -0.95f, 0.7f);
    }

    void PlayerController::Start() {
        InputController* input = EngineController::get_input();
        
        // Toggle on 'E' press
        input->subscribe_press_button(GLFW_KEY_E, std::bind(&PlayerController::toggle_mode, this));

        this->ship->disable();
    }

    void PlayerController::Update() {
        bool is_humanoid = this->humanoid->is_enabled();

        // Destroy ship if rolling too fast
        if (!is_humanoid && std::fabs(this->ship->get_angular_velocity()->euler_angles().z) > this->ship->get_critical_roll_velocity()) {
            this->toggle_mode();
        }

        // Make ship not dirty when grounded
        if (is_humanoid && this->humanoid->get_walker()->is_grounded()) {
            this->ship_is_dirty = false;
        }
    }

    void PlayerController::PostUpdate() {

        this->time += EngineController::get_delta_time();

        if (this->is_game_over) {
            this->game_over_routine();
            return;
        }

        float delta_time = EngineController::get_delta_time();
        // Decrease oxygen level over time if it is humanoid and is not grouded or is in the ship but there is no fuel

        bool is_humanoid_active = this->humanoid && this->humanoid->is_enabled();
        bool is_ship_active = this->ship && this->ship->is_enabled();
        if ((is_humanoid_active && !this->humanoid->get_walker()->is_grounded()) || (is_ship_active && this->ship->get_ship_controller()->fuel <= 0.0f)) {
            this->oxygen_level -= this->oxygen_decrease_rate * delta_time;
        } else {
            // Recover oxygen when grounded
            this->oxygen_level += (this->oxygen_recovery_rate) * delta_time;
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
            this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, -0.95f, 0.9f);
        }

        // Update HUD oxygen text
        if (this->oxygen_text && this->humanoid) {
            std::ostringstream oxy_ss;;
            oxy_ss << std::fixed << std::setprecision(1) << this->oxygen_level;
            this->oxygen_text->setText(std::string("Oxygen: ") + oxy_ss.str(), 1.5f, -0.95f, 0.8f);
        }

        // game over logic
        if (this->oxygen_level <= 0.0f) {
            this->game_over(std::string("OXYGEN DEPLETED"));
        } else if (this->ship->collided_with_planets()) {
            this->game_over(std::string("SHIP CRASHED"));
        } else if (this->killed_by_enemy) {
            this->game_over(std::string("KILLED BY ENEMY"));
        }
    }

    void PlayerController::restart() {
        // reset spaceship velocities
        this->time_text->setText("", 1.5f, -0.95f, 0.7f);
        this->ship->get_kinematic_body()->set_velocity(Vec3(0.0f, 0.0f, 0.0f));
        this->ship->get_angular_velocity()->reset();
        this->humanoid->get_walker()->set_velocity(Vec3(0.0f, 0.0f, 0.0f));
        this->humanoid->get_walker()->reset_grounded_to();
        this->is_game_over = false;
        this->game_over_timer = 0.0f;
        this->oxygen_level = 100.0f;
        this->killed_by_enemy = false;
        this->game_over_text->setText(std::string(""), 1.8f, 0.0f, 0.0f);
        this->humanoid->enable();
        this->ship->disable();
        this->ship->get_ship_controller()->set_fuel(100.0f);
        this->ship_is_dirty = false;
        Camera::set_main(this->humanoid_cam);
        this->time = 0.0f;
        scenes::MainScene::restart();
    }

    void PlayerController::hit_by_enemy() {
        this->killed_by_enemy = true;
    }

    void PlayerController::toggle_mode() {
        if (!this->humanoid || !this->ship) return;
        if (this->humanoid->get_walker()->is_grounded()) return; // Only allow switching when humanoid is not grounded

        bool was_humanoid = this->humanoid->is_enabled();
        if (was_humanoid) {
            bool ship_is_blocked =
                // Can't switch to ship if out of fuel
                this->ship->get_ship_controller()->fuel <= 0.0f
                
                // Can't switch to ship if ship is dirty
                || this->ship_is_dirty;

            
            if (ship_is_blocked) {
                return;
            }

            float phi = this->humanoid->get_camera_phi();
            Quaternion rotation_adjustment = Quaternion::from_x_rotation(phi);
            this->get_vobject()->transform().local_quaternion().local_compose(rotation_adjustment);
            this->humanoid->reset_camera_phi();
            
            this->humanoid->disable();
            this->ship->enable();
            Camera::set_main(this->ship_cam);

            this->ship_is_dirty = true;
        } else {
            this->ship->disable();
            this->humanoid->enable();
            Camera::set_main(this->humanoid_cam);
        }
    }

    void PlayerController::game_over(std::string reason) {
        this->humanoid->disable();
        this->ship->disable();
        Camera::set_main(this->humanoid_cam);

        std::string full_text = "GAME OVER: " + reason;

        float font_size = 2.0f;
        float char_width = game_over_text->get_text_width();
        float text_width = static_cast<float>(full_text.length()) * char_width * font_size;
        float x_pos = -text_width * 0.5f;
        float y_pos = 0.0f;
        if (x_pos < -1.0f) x_pos = -1.0f;
        if (x_pos > 1.0f) x_pos = 1.0f;
        this->game_over_text->setText(full_text, font_size, x_pos, y_pos);

        std::string time_text_str = "Survived time: ";

        font_size = 1.5f;
        char_width = game_over_text->get_text_width();
        x_pos = -text_width * 0.5f;
        if (x_pos < -1.0f) x_pos = -1.0f;
        if (x_pos > 1.0f) x_pos = 1.0f;
        this->time_text->setText(time_text_str + std::to_string(static_cast<int>(this->time)) + "s", font_size, x_pos, y_pos - 0.1f);

        this->is_game_over = true;
    }

    void PlayerController::game_over_routine() {
        this->game_over_timer += EngineController::get_delta_time();
        if (this->game_over_timer < this->time_to_restart) {
            return;
        }
        this->restart();
    }
}
