#include "AngularVelocity.hpp"
#include <engine>

using namespace engine;

namespace game::components {
    
    void AngularVelocity::Update() {
        float dt = engine::EngineController::get_delta_time();
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quat = transform.local_quaternion();
        quat.local_compose(Quaternion::from_euler_zyx(this->_euler_angles * dt));
        quat.normalize();
    }

} // namespace game::components
