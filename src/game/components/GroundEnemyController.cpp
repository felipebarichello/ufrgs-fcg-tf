#include "GroundEnemyController.hpp"
#include <engine>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstdio>
#include <game/components/WalkerController.hpp>

using engine::EngineController;
using engine::Vec2;
using engine::Vec3;
using engine::Transform;
using engine::math::Quaternion;

namespace game::components {

    void GroundEnemyController::Awake() {
        // Keep reference to target VObject for positional queries
        this->target_vobj = this->target_controller->get_vobject();

        // Create a WalkerController for this enemy and attach it to our VObject so it handles physics
        this->walker = new WalkerController(this->planets);
        // Add the walker component to this VObject so it will be scheduled by the scene
        this->get_vobject()->add_component(std::unique_ptr<engine::Component>(this->walker));
    }

    void GroundEnemyController::Update() {
        if (this->target_vobj == nullptr) return;

        // Compute an AI input vector and forward to our walker
        this->update_follow_target();
    }

    void GroundEnemyController::update_follow_target() {
        // Compute 2D input vector (forward,right) in local space and forward to walker
        Transform& transform = this->get_vobject()->transform();

        Vec3 my_pos = transform.get_position();
        Vec3 target_pos = this->target_vobj->transform().get_position();

        Vec3 to_target = target_pos - my_pos;
        float dist = glm::length(to_target);
        if (dist < 1e-6f) {
            if (this->walker) this->walker->set_move_vector(Vec2(0.0f, 0.0f));
            return;
        }

        Vec3 dir = to_target / dist;

        // Project direction onto local tangent (use the VObject's current up as the surface normal proxy).
        Vec3 up = transform.quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        Vec3 dir_horiz = dir - glm::dot(dir, up) * up;
        float len = glm::length(dir_horiz);
        Vec3 projected = (len < 1e-6f) ? Vec3(0.0f) : dir_horiz / len;

        // Determine local forward/right basis
        Vec3 forward = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right = transform.quaternion().rotate(Vec3(1.0f, 0.0f, 0.0f));

        float forward_comp = glm::dot(projected, forward);
        float right_comp = glm::dot(projected, right);

        Vec2 move_vec(right_comp, forward_comp);
        if (this->walker) this->walker->set_move_vector(move_vec);
    }

} // namespace game::components
