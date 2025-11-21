#include "PlayerSwitcherController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>

using engine::EngineController;
using engine::InputController;
using engine::Camera;
using engine::Transform;
using engine::Quaternion;

namespace game::components {

    void PlayerSwitcherController::Start() {
        InputController* input = EngineController::get_input();
        
        // Toggle on 'E' press
        input->subscribe_press_button(GLFW_KEY_E, std::bind(&PlayerSwitcherController::toggle_active, this));

        this->spaceship->disable();
    }

    void PlayerSwitcherController::toggle_active() {
        if (!this->humanoid || !this->spaceship) return;
        if (this->humanoid->get_walker()->is_grounded()) return; // Only allow switching when humanoid is not grounded

        bool was_humanoid = this->humanoid->is_active();
        if (was_humanoid) {
            this->humanoid->disable();
            this->spaceship->enable();
            Camera::set_main(this->ship_cam);
        } else {
            this->spaceship->disable();
            this->humanoid->enable();
            Camera::set_main(this->humanoid_cam);
        }
    }

}
