#include "PlayerController.hpp"
#include <engine>

using engine::EngineController;
using engine::Vec3;

namespace game::components {
    void PlayerController::Start() {
        EngineController::get_input()->subscribe_dpad(&this->move_vector, GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J, GLFW_KEY_L);
    }

    void PlayerController::Update() {
        auto velocity = this->speed * this->move_vector;
        this->get_vobject()->transform().add_position(Vec3(velocity.x, 0.0f, velocity.y));
    }
}
