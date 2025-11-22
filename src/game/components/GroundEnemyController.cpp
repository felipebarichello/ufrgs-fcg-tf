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
using engine::Quaternion;

namespace game::components {

    void GroundEnemyController::Awake() {
        // Keep reference to target VObject for positional queries
        this->target_vobj = this->target_controller->get_vobject();
    }

    void GroundEnemyController::Update() {
        // Compute an AI input vector and forward to our walker
        this->update_follow_target();
    }

    void GroundEnemyController::update_follow_target() {
        // Compute 2D input vector (forward,right) in local space and forward to walker
        Transform& transform = this->get_vobject()->transform();

        Vec3 my_pos = transform.get_world_position();
        Vec3 target_pos = this->target_vobj->transform().get_world_position();

        Vec3 to_target = target_pos - my_pos;
        float dist = engine::h_norm(to_target);
        if (dist < 1e-6f) {
            if (this->walker) this->walker->set_move_vector(Vec2(0.0f, 0.0f));
            return;
        }

        Vec3 dir = to_target / dist;

        // Project direction onto local tangent (use the VObject's current up as the surface normal proxy).
        Vec3 up = transform.quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        Vec3 dir_horiz = dir - engine::h_dot_product(dir, up) * up;
        float len = engine::h_norm(dir_horiz);
        Vec3 projected = (len < 1e-6f) ? Vec3(0.0f) : dir_horiz / len;

        // Determine local forward/right basis
        Vec3 forward = transform.quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right = transform.quaternion().rotate(Vec3(1.0f, 0.0f, 0.0f));

        float forward_comp = engine::h_dot_product(projected, forward);
        float right_comp = engine::h_dot_product(projected, right);

        Vec2 move_vec(right_comp, forward_comp);
        if (this->walker) this->walker->set_move_vector(move_vec);

        // Use the target controller's walker/collider to detect a proper collision
        if (this->target_controller) {
            WalkerController* target_walker = this->target_controller->get_walker();
            if (target_walker) {
                auto enemy_cyl = this->get_cylinder_collider();
                auto target_cyl = this->target_controller->get_cylinder_collider();

                if (engine::collision::collide_cylinder_cylinder(*enemy_cyl, *target_cyl).has_collided()) {
                    this->target_controller->hit_by_enemy();
                }
            }
        }

        // Also rotate the VObject to face the movement direction (projected)
        Quaternion& quaternion = transform.quaternion();
        // Recompute forward projection onto tangent plane
        Vec3 forward_proj = forward - engine::h_dot_product(forward, up) * up;
        float fwd_len = engine::h_norm(forward_proj);
        float proj_len = engine::h_norm(projected);
        if (fwd_len > 1e-6f && proj_len > 1e-6f) {
            Vec3 fwd_n = forward_proj / fwd_len;
            Vec3 proj_n = projected / proj_len;

            // Rotation that brings fwd_n -> proj_n
            Quaternion face_rot = Quaternion::from_unit_vectors(fwd_n, proj_n);
            // Apply rotation in global frame so the up direction remains consistent
            quaternion.global_compose(face_rot);
            quaternion.normalize();
        }
    }

} // namespace game::components
