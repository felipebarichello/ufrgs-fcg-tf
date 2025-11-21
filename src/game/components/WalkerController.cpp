#include "WalkerController.hpp"
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

    void WalkerController::Update() {
        if (!this->active) return;

        // Apply input-driven walking only when grounded
        this->update_transform_due_to_input();
        // Environment (gravity, collisions, integration)
        this->update_transform_due_to_environment();
    }

    void WalkerController::PostUpdate() {
        Transform& transform = this->get_vobject()->transform();

        // Test and correct collisions
        if (this->grounded_to.has_value()) {
            // If grounded, snap back to the surface when moving because you should not walk away from the planet
            PlanetInfo* planet = this->grounded_to.value();
            Vec3 planet_position = planet->get_vobject()->transform().get_position();
            Vec3 direction_from_planet = glm::normalize(transform.get_position() - planet_position);
            transform.position() = planet_position + direction_from_planet * planet->get_radius();
            
            // Remove vertical component of velocity
            Vec3 up_direction = direction_from_planet;
            Vec3 vertical_velocity = glm::dot(this->kinematic->get_velocity(), up_direction) * up_direction;
            this->kinematic->set_velocity(this->kinematic->get_velocity() - vertical_velocity);
        } else {
            // If not grounded, check for collision
            this->correct_planet_collision();
        }

        // Align rotation to planet
        this->align_to_closest_planet();
    }

    void WalkerController::update_transform_due_to_input() {
        // Only handle walking when grounded
        Transform& transform = this->get_vobject()->transform();
        if (!this->grounded_to.has_value()) return;

        auto& quaternion = transform.quaternion();

        Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        Vec3 move_vector_3d = this->move_vector.y * front_of_player;
        move_vector_3d += this->move_vector.x * right_of_player;
        Vec3 desired_velocity_change = this->walk_accel * move_vector_3d * EngineController::get_delta_time();
        Vec3 desired_velocity = this->kinematic->get_velocity() + desired_velocity_change;
        float desired_speed = glm::length(desired_velocity);

        if (desired_speed > this->max_walk_speed) {
            // Can't walk faster. Correct that.
            Vec3 velocity_direction = glm::normalize(this->kinematic->get_velocity());
            Vec3 clamped_cur_vel = velocity_direction * this->max_walk_speed;
            Vec3 excess_velocity = this->kinematic->get_velocity() - clamped_cur_vel;

            // Walking can only operate on the non-excess part
            Vec3 desired_vel_component = clamped_cur_vel + desired_velocity_change;
            float vel_component_speed = glm::length(desired_vel_component);
            if (vel_component_speed > this->max_walk_speed) {
                // Clamp to max walk speed
                desired_vel_component = desired_vel_component / vel_component_speed * this->max_walk_speed;
            }

            desired_velocity = desired_vel_component + excess_velocity;
        }

        { // Deaccelerate in the direction orthogonal to movement and when stopped (and also when moving opposite)
            Vec3 up = transform.quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
            Vec3 horizontal_velocity = this->kinematic->get_velocity() - glm::dot(this->kinematic->get_velocity(), up) * up;

            if (engine::is_zero(move_vector_3d)) {
                // No input, deaccelerate fully
                float horizontal_speed = glm::length(horizontal_velocity);
                if (horizontal_speed > 1e-6f) {
                    Vec3 horizontal_direction = horizontal_velocity / horizontal_speed;
                    float deaccel_amount = std::min(horizontal_speed, this->walk_deaccel * EngineController::get_delta_time());
                    desired_velocity -= deaccel_amount * horizontal_direction;
                }
            } else {
                // Input present: deaccelerate orthogonal component and any component opposite to the input
                Vec3 input_direction = glm::normalize(move_vector_3d);

                float horiz_speed = glm::length(horizontal_velocity);
                if (horiz_speed > 1e-6f) {
                    // scalar component along input (can be negative if moving opposite)
                    float scalar_along = glm::dot(horizontal_velocity, input_direction);

                    // orthogonal component (horizontal_velocity minus its projection onto input_direction)
                    Vec3 orthogonal_vec = horizontal_velocity - scalar_along * input_direction;
                    float orthogonal_speed = glm::length(orthogonal_vec);

                    float dt = EngineController::get_delta_time();

                    // Deaccelerate orthogonal component
                    if (orthogonal_speed > 1e-6f) {
                        Vec3 orthogonal_dir = orthogonal_vec / orthogonal_speed;
                        float deaccel_amount = std::min(orthogonal_speed, this->walk_deaccel * dt);
                        desired_velocity -= deaccel_amount * orthogonal_dir;
                    }

                    // If moving opposite to input, deaccelerate that opposite component towards zero
                    if (scalar_along < -1e-6f) {
                        float opposite_speed = -scalar_along;
                        float deaccel_amount = std::min(opposite_speed, this->walk_deaccel * dt);
                        // Move velocity along input direction (positive) to reduce the negative component
                        desired_velocity += deaccel_amount * input_direction;
                    }
                }
            }
        }
        
        this->kinematic->set_velocity(desired_velocity);

        // Handle jump request
        if (this->jump_requested) {
            this->jump_requested = false;
            if (this->grounded_to.has_value()) {
                this->kinematic->set_velocity(this->kinematic->get_velocity() + this->jump_strength * this->get_vobject()->transform().quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f)));
                this->grounded_to = std::nullopt;
            }
        }
    }

    void WalkerController::update_transform_due_to_environment() {
        // Apply gravity when not grounded
        if (!this->grounded_to.has_value()) {
            Vec3 equivalent_gravity = this->compute_equivalent_gravity();
            this->kinematic->set_velocity(this->kinematic->get_velocity() + equivalent_gravity * EngineController::get_delta_time());
        }
    }

    engine::Vec3 WalkerController::compute_equivalent_gravity() {
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

        return gravity_sum * WalkerController::GRAVITATIONAL_CONSTANT;
    }

    void WalkerController::correct_planet_collision() {
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

            if (engine::collision::collide_point_sphere(*this->get_point_collider(), *planet->get_sphere_collider()).has_collided()) {
                // Collision detected. Correcting...
                Vec3 direction_to_planet = glm::normalize(vec_to_planet);
                Vec3 velocity_to_planet = glm::dot(this->kinematic->get_velocity(), direction_to_planet) * direction_to_planet;

                if (glm::length(velocity_to_planet) > 0.0f) {
                    this->kinematic->set_velocity(this->kinematic->get_velocity() - velocity_to_planet);
                }

                transform.position() = planet_position - direction_to_planet * planet->get_radius();
                this->grounded_to = planet;
                return;
            }
        }
    }

    void WalkerController::align_to_closest_planet() {
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
