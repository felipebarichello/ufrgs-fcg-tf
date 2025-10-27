#include "GroundEnemyController.hpp"
#include <engine>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstdio>

using engine::EngineController;
using engine::Vec2;
using engine::Vec3;
using engine::Transform;
using engine::math::Quaternion;

namespace game::components {

    void GroundEnemyController::Awake() {
        this->target_vobj = this->target_controller->get_vobject();
    }

    void GroundEnemyController::Update() {
        // Basic flow: follow target, then environment (gravity, collision, alignment)
        if (this->target_vobj == nullptr) return;

        this->update_follow_target();
        this->update_transform_due_to_environment();
    }

    void GroundEnemyController::update_follow_target() {
        // Move towards the target using acceleration and simple speed clamping.
        Transform& transform = this->get_vobject()->transform();

        Vec3 my_pos = transform.get_position();
        Vec3 target_pos = this->target_vobj->transform().get_position();

        Vec3 to_target = target_pos - my_pos;
        float dist = glm::length(to_target);
        if (dist < 1e-6f) return;

        Vec3 dir = to_target / dist;

        // If grounded, project direction onto tangent plane so the enemy walks on the surface
        if (this->grounded_to.has_value()) {
            PlanetInfo* planet = this->grounded_to.value();
            Vec3 planet_pos = planet->get_vobject()->transform().get_position();
            Vec3 up = glm::normalize(transform.get_position() - planet_pos);

            // remove component along up
            Vec3 dir_horiz = dir - glm::dot(dir, up) * up;
            float len = glm::length(dir_horiz);
            if (len < 1e-6f) {
                // target is exactly above/below -> no horizontal movement
                return;
            }
            dir = dir_horiz / len;
        }

        float dt = EngineController::get_delta_time();

        // Accelerate towards dir on the horizontal/tangent component
        this->current_velocity += dir * (this->move_accel * dt);

        // If grounded, apply deacceleration to horizontal/tangent components
        if (this->grounded_to.has_value()) {
            PlanetInfo* planet = this->grounded_to.value();
            Vec3 planet_pos = planet->get_vobject()->transform().get_position();
            Vec3 up = glm::normalize(transform.get_position() - planet_pos);

            // Separate horizontal (tangent) and vertical components
            Vec3 horizontal_velocity = this->current_velocity - glm::dot(this->current_velocity, up) * up;

            // If we're very close to the target, deaccelerate horizontally to stop
            const float stop_distance = 1.0f;
            if (dist <= stop_distance) {
                float horizontal_speed = glm::length(horizontal_velocity);
                if (horizontal_speed > 1e-6f) {
                    Vec3 horizontal_dir = horizontal_velocity / horizontal_speed;
                    float deaccel_amount = std::min(horizontal_speed, this->move_deaccel * dt);
                    this->current_velocity -= deaccel_amount * horizontal_dir;
                }
            } else {
                // Otherwise, deaccelerate orthogonal component and any component opposite to the desired direction
                Vec3 input_direction = dir; // already projected earlier when necessary

                float horiz_speed = glm::length(horizontal_velocity);
                if (horiz_speed > 1e-6f) {
                    float scalar_along = glm::dot(horizontal_velocity, input_direction);

                    // orthogonal component (horizontal_velocity minus its projection onto input_direction)
                    Vec3 orthogonal_vec = horizontal_velocity - scalar_along * input_direction;
                    float orthogonal_speed = glm::length(orthogonal_vec);

                    // Remove orthogonal component up to deaccel amount
                    if (orthogonal_speed > 1e-6f) {
                        float deaccel_amount = std::min(orthogonal_speed, this->move_deaccel * dt);
                        Vec3 orth_dir = orthogonal_vec / orthogonal_speed;
                        horizontal_velocity -= deaccel_amount * orth_dir;
                    }

                    // If moving opposite to desired direction, reduce that component as well
                    if (scalar_along < 0.0f) {
                        float opp_amount = std::min(-scalar_along, this->move_deaccel * dt);
                        horizontal_velocity += opp_amount * input_direction; // add because scalar_along is negative
                    }
                }

                // Recompose velocity keeping vertical component
                Vec3 vertical = glm::dot(this->current_velocity, up) * up;
                this->current_velocity = horizontal_velocity + vertical;
            }

            // Clamp horizontal speed after deaccel
            horizontal_velocity = this->current_velocity - glm::dot(this->current_velocity, up) * up;
            float hspeed = glm::length(horizontal_velocity);
            if (hspeed > this->max_move_speed) {
                horizontal_velocity = horizontal_velocity / hspeed * this->max_move_speed;
                Vec3 vertical = glm::dot(this->current_velocity, up) * up;
                this->current_velocity = horizontal_velocity + vertical;
            }
        } else {
            // In free space, just clamp total speed
            float speed = glm::length(this->current_velocity);
            if (speed > this->max_move_speed) {
                this->current_velocity = this->current_velocity / speed * this->max_move_speed;
            }
        }
    }

    void GroundEnemyController::update_transform_due_to_environment() {
        Transform& transform = this->get_vobject()->transform();

        // Apply gravity when not grounded
        if (!this->grounded_to.has_value()) {
            Vec3 equivalent_gravity = this->compute_equivalent_gravity();
            this->current_velocity += equivalent_gravity * EngineController::get_delta_time();
        }

        // Integrate
        transform.position() += this->current_velocity * EngineController::get_delta_time();

        if (this->grounded_to.has_value()) {
            // snap to surface and remove vertical velocity
            PlanetInfo* planet = this->grounded_to.value();
            Vec3 planet_position = planet->get_vobject()->transform().get_position();
            Vec3 direction_from_planet = glm::normalize(transform.get_position() - planet_position);
            transform.position() = planet_position + direction_from_planet * planet->get_radius();

            Vec3 up_direction = direction_from_planet;
            Vec3 vertical_velocity = glm::dot(this->current_velocity, up_direction) * up_direction;
            this->current_velocity -= vertical_velocity;
        } else {
            // If not grounded, check collision
            this->correct_planet_collision();
        }

        this->align_to_closest_planet();
    }

    engine::Vec3 GroundEnemyController::compute_equivalent_gravity() {
        Vec3 gravity_sum(0.0f);
        Transform& transform = this->get_vobject()->transform();
        for (PlanetInfo* planet : this->planets) {
            Vec3 vec_to_planet = planet->get_vobject()->transform().get_position() - transform.get_position();
            float distance_to_planet = glm::length(vec_to_planet);
            if (distance_to_planet > 1e-6f) {
                Vec3 grav_direction = glm::normalize(vec_to_planet);
                gravity_sum += (planet->get_gravity_mass() / distance_to_planet) * grav_direction;
            }
        }

        return gravity_sum * GroundEnemyController::GRAVITATIONAL_CONSTANT;
    }

    void GroundEnemyController::correct_planet_collision() {
        // No need to keep checking if already grounded
        if (this->grounded_to.has_value()) {
            return;
        }

        Transform& transform = this->get_vobject()->transform();

        this->closest_planet = nullptr;
        float closest_planet_distance = INFINITY;
        for (PlanetInfo* planet : this->planets) {
            Vec3 planet_position = planet->get_vobject()->transform().get_position();
            Vec3 vec_to_planet = planet_position - transform.get_position();
            float distance_to_planet = glm::length(vec_to_planet);

            if (distance_to_planet < closest_planet_distance) {
                this->closest_planet = planet;
                closest_planet_distance = distance_to_planet;
            }

            if (distance_to_planet < planet->get_radius()) {
                // Collision detected. Correcting...
                Vec3 direction_to_planet = glm::normalize(vec_to_planet);
                Vec3 velocity_to_planet = glm::dot(this->current_velocity, direction_to_planet) * direction_to_planet;

                if (glm::length(velocity_to_planet) > 0.0f) {
                    this->current_velocity -= velocity_to_planet;
                }

                transform.position() = planet_position - direction_to_planet * planet->get_radius();
                this->grounded_to = planet;
                return;
            }
        }
    }

    void GroundEnemyController::align_to_closest_planet() {
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quaternion = transform.quaternion();
        Vec3 current_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));

        if (this->grounded_to.has_value()) {
            PlanetInfo* grounded_planet = this->grounded_to.value();
            Vec3 grounded_planet_position = grounded_planet->get_vobject()->transform().get_position();
            Vec3 vec_to_grounded_planet = grounded_planet_position - transform.get_position();
            Vec3 grounded_up_direction = -glm::normalize(vec_to_grounded_planet);

            Quaternion align_quat = Quaternion::from_unit_vectors(current_up, grounded_up_direction);
            quaternion.global_compose(align_quat);
            quaternion.normalize();
            return;
        }

        if (this->closest_planet == nullptr) return;

        Vec3 closest_planet_position = this->closest_planet->get_vobject()->transform().get_position();
        Vec3 vec_to_closest_planet = closest_planet_position - transform.get_position();
        Vec3 closest_surface_point = closest_planet_position - glm::normalize(vec_to_closest_planet) * this->closest_planet->get_radius();
        Vec3 vec_to_closest_point = closest_surface_point - transform.get_position();
        float closest_point_distance = glm::length(vec_to_closest_point);

        // Do a soft alignment when near
        float MAX_SURFACE_ALIGN_DISTANCE = 200.0f;
        float MIN_SURFACE_ALIGN_DISTANCE = 2.0f;

        if (closest_point_distance > MAX_SURFACE_ALIGN_DISTANCE) return;

        Quaternion align_quat = Quaternion::from_unit_vectors(current_up, -glm::normalize(vec_to_closest_planet));

        if (closest_point_distance <= MIN_SURFACE_ALIGN_DISTANCE) {
            quaternion.global_compose(align_quat);
            quaternion.normalize();
            return;
        }

        float planet_alignment_force = (MAX_SURFACE_ALIGN_DISTANCE - closest_point_distance) / (MAX_SURFACE_ALIGN_DISTANCE - MIN_SURFACE_ALIGN_DISTANCE);
        float frame_planet_alignment = powf(planet_alignment_force, 16.0f);
        Quaternion partial_align_quat = Quaternion::slerp(Quaternion::identity(), align_quat, frame_planet_alignment);
        quaternion.global_compose(partial_align_quat);
        quaternion.normalize();
    }

} // namespace game::components
