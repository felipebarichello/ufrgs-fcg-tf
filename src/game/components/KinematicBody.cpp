#include "KinematicBody.hpp"
#include <engine>

using engine::Transform;

namespace game::components {
    
    void KinematicBody::Update() {
        Transform& transform = this->get_vobject()->transform();

        // Simple position integration
        float dt = engine::EngineController::get_delta_time();
        transform.set_world_position(transform.get_world_position() + this->velocity * dt);
    }

} // namespace game::components
