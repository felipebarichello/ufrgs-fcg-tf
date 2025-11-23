#include "AngularVelocity.hpp"
#include <engine>

using namespace engine;

namespace game::components {
    
    void AngularVelocity::Update() {
        float dt = engine::EngineController::get_delta_time();
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quat = transform.quaternion();
        quat.local_compose(Quaternion::from_z_rotation(-this->_ang_velocity * dt));
        quat.normalize();
    }

} // namespace game::components
