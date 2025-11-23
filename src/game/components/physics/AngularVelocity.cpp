#include "AngularVelocity.hpp"
#include <engine>

using namespace engine;

namespace game::components {
    
    void AngularVelocity::Update() {
        float dt = engine::EngineController::get_delta_time();
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quat = transform.quaternion();
        quat.local_compose(Quaternion::from_x_rotation(this->_euler_angles.x * dt));
        quat.local_compose(Quaternion::from_y_rotation(this->_euler_angles.y * dt));
        quat.local_compose(Quaternion::from_z_rotation(-this->_euler_angles.z * dt));
        quat.normalize();
    }

} // namespace game::components
