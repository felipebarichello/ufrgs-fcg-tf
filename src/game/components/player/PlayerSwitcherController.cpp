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
    }

    void PlayerSwitcherController::toggle_active() {
        if (!this->humanoid || !this->spaceship) return;
        if (this->humanoid->get_walker()->is_grounded()) return; // Only allow switching when humanoid is not grounded

        bool was_humanoid = this->humanoid->is_active();
        Transform& human_transform = this->humanoid->get_vobject()->transform();
        Transform& ship_transform = this->spaceship->get_vobject()->transform();

        // Swap main camera if cameras provided
        if (this->humanoid_cam && this->ship_cam) {
            if (was_humanoid) {
                this->humanoid->disable();
                
                ship_transform.position() = human_transform.position();
                ship_transform.quaternion() = human_transform.get_quaternion();

                this->spaceship->enable();

                Camera::set_main(this->ship_cam);
            } else {
                this->spaceship->disable();

                human_transform.position() = ship_transform.position();
                human_transform.quaternion() = ship_transform.get_quaternion();

                this->humanoid->enable();
                
                Camera::set_main(this->humanoid_cam);
            }
        }
    }

}
