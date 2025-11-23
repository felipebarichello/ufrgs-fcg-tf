#include "PlayerController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>
#include <sstream>
#include <iomanip>
#include <game/scenes/MainScene_vars.hpp>

using namespace engine;

namespace game::components {

    void PlayerController::Awake() {
        // Texts setup

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
        init_ss << std::fixed << std::setprecision(1) << this->ship->get_ship_controller()->get_fuel();
        this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
        this->game_over_text->setText(std::string(""), 3.0f, engine::Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
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
        // Update HUD fuel text
        if (this->fuel_text) {
            std::ostringstream init_ss;
            init_ss << std::fixed << std::setprecision(1) << this->ship->get_ship_controller()->get_fuel();
            this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
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
            this->humanoid->disable();

            float phi = this->humanoid->get_camera_phi();
            Quaternion rotation_adjustment = Quaternion::from_x_rotation(phi);
            this->get_vobject()->transform().quaternion().local_compose(rotation_adjustment);
            this->humanoid->reset_camera_phi();

            this->ship->enable();
            Camera::set_main(this->ship_cam);
        } else {
            this->ship->disable();
            this->humanoid->enable();
            Camera::set_main(this->humanoid_cam);
        }
    }

}
