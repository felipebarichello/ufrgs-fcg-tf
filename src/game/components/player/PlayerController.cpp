#include "PlayerController.hpp"
#include <InputController.hpp>
#include <EngineController.hpp>

using engine::EngineController;
using engine::InputController;
using engine::Camera;
using engine::Transform;
using engine::Quaternion;

namespace game::components {

    void PlayerController::Start() {
        InputController* input = EngineController::get_input();
        
        // Toggle on 'E' press
        input->subscribe_press_button(GLFW_KEY_E, std::bind(&PlayerController::toggle_mode, this));

        this->ship->disable();
    }

    void PlayerController::Update() {
        // Destroy ship if rolling too fast
        if (this->ship->is_enabled() && std::fabs(this->ship->get_angular()->euler_angles().z) > this->ship->get_critical_roll_velocity()) {
            this->toggle_mode();
        }
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
