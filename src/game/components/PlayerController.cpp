#include "PlayerController.hpp"
#include <engine>

using engine::EngineController;
using engine::Vec3;

namespace game::components {
    void PlayerController::Start() {
        EngineController::get_input()->subscribe_dpad(&this->move_vector, GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J, GLFW_KEY_L);
    }

    void PlayerController::Update() {
        // auto velocity = this->speed * this->move_vector;
        // // move in local X (right) / Y (forward) relative to player yaw
        // auto& tf = this->get_vobject()->transform();
        // // Extract yaw (rotation around Y) from the quaternion by converting
        // // to Euler ZYX (returns angles in order Z, Y, X). We only need Y.
        // engine::Vec3 eulerZYX = tf.get_quaternion().toEulerZYX();
        // float yaw = eulerZYX.y; // assume Y is up (radians)
        // float c = std::cos(yaw), s = std::sin(yaw);
        // float wx = velocity.x * c - velocity.y * s;
        // float wz = velocity.x * s + velocity.y * c;
        // tf.position() = Vec3(wx, 0.0f, wz);
    }
}
