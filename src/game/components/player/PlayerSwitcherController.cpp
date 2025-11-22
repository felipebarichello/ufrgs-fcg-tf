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

        bool was_humanoid = this->humanoid->is_enabled();
        if (was_humanoid) {
            this->humanoid->disable();

            // float phi = this->humanoid->get_camera_phi();
            // Quaternion rotation_adjustment = Quaternion::from_x_rotation(-phi);
            // this->get_vobject()->transform().quaternion().local_compose(rotation_adjustment);

            this->spaceship->enable();
            Camera::set_main(this->ship_cam);
        } else {
            this->spaceship->disable();
            this->humanoid->enable();
            Camera::set_main(this->humanoid_cam);
        }
    }

}
