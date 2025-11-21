#include "PlayerSwitcherController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>

using engine::EngineController;
using engine::InputController;

namespace game::components {

    void PlayerSwitcherController::Start() {
        InputController* input = EngineController::get_input();
        
        // Toggle on 'E' press
        input->subscribe_press_button(GLFW_KEY_E, std::bind(&PlayerSwitcherController::toggle_active, this));
    }

    void PlayerSwitcherController::toggle_active() {
        if (!this->humanoid || !this->spaceship) return;

        bool humanoid_was = this->humanoid->is_active();
        this->humanoid->set_active(!humanoid_was);
        this->spaceship->set_active(humanoid_was);

        // Swap main camera if cameras provided
        if (this->humanoid_cam && this->ship_cam) {
            if (!humanoid_was) {
                // humanoid was inactive and now active -> set humanoid camera
                engine::Camera::set_main(this->humanoid_cam);
            } else {
                engine::Camera::set_main(this->ship_cam);
            }
        }
    }

}
