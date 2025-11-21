#include "KinematicBody.hpp"

namespace game::components {
    
    void KinematicBody::Update() {
        // Simple position integration
        float dt = engine::EngineController::get_delta_time();
        this->get_vobject()->transform().position() += this->velocity * dt;
    }

} // namespace game::components
