#include "PlayerController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <game/components/PlanetInfo.hpp>

using engine::EngineController;
using engine::InputController;
using engine::Vec2;
using engine::Vec3;
using engine::math::Quaternion;
using engine::is_zero;
using engine::Transform;

namespace game::components {
    struct PlayerController::SphericalInput {
        float delta_theta;
        float delta_phi;
    };

    void PlayerController::Start() {
        InputController* input = EngineController::get_input();
        input->subscribe_dpad(&this->move_vector_2d, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        input->subscribe_press_button(GLFW_KEY_F6, std::bind(&PlayerController::toggle_camera_release, this));
        input->subscribe_press_button(GLFW_KEY_SPACE, std::bind(&PlayerController::jump, this));
    }

    void PlayerController::Update() {
        if (this->released_camera) {
            this->update_released_camera();
        } else {
            this->update_transform_due_to_input();
        }

        this->update_transform_due_to_environment();
    }

    void PlayerController::update_transform_due_to_environment() {
        Transform& transform = this->get_vobject()->transform();

        /* Position change caused by gravity */

        // Calculate (vector) gravity sum from all planets.
        // Each planet contributes (mass / distance) * direction_to_planet.
        Vec3 gravity_sum {0.0f, 0.0f, 0.0f};
        Vec3 equivalent_gravity(0.0f);

        if (!this->grounded_to.has_value()) {
            equivalent_gravity = this->compute_equivalent_gravity();
            this->current_velocity += equivalent_gravity * EngineController::get_delta_time();
        }

        // TODO: Is this physically accurate?
        transform.position() += this->current_velocity * EngineController::get_delta_time();
        this->correct_planet_collision();

        /* Direction change due to gravity or planet alignment */

        this->align_to_closest_planet();
    }

    void PlayerController::update_transform_due_to_input() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();


        /* Camera (attached) movement */

        PlayerController::SphericalInput spherical = this->get_spherical_input();
        this->set_camera_phi(this->camera_phi + spherical.delta_phi);
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));


        /* Walking movement */

        // Can only walk if grounded
        if (this->grounded_to.has_value()) {
            Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
            Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

            Vec3 move_vector_3d = this->move_vector_2d.y * front_of_player;
            move_vector_3d += this->move_vector_2d.x * right_of_player;
            Vec3 desired_velocity_change = this->walk_accel * move_vector_3d * EngineController::get_delta_time();
            Vec3 desired_velocity = this->current_velocity + desired_velocity_change;
            float desired_speed = glm::length(desired_velocity);

            if (desired_speed > this->max_walk_speed) {
                // Can't walk faster. Correct that.
                Vec3 velocity_direction = glm::normalize(this->current_velocity);
                Vec3 max_velocity_component = velocity_direction * this->max_walk_speed;
                Vec3 excess_velocity = desired_velocity - max_velocity_component;
                
                // Walking can only operate on the non-excess part
                Vec3 desired_vel_component = max_velocity_component + desired_velocity_change;
                float vel_component_speed = glm::length(desired_vel_component);
                if (vel_component_speed > this->max_walk_speed) {
                    // Clamp to max walk speed
                    desired_vel_component = desired_vel_component / vel_component_speed * this->max_walk_speed;
                }

                desired_velocity = desired_vel_component + excess_velocity;
            }

            { // Deaccelerate in the direction orthogonal to movement and when stopped (and also when moving opposite)
                Vec3 up = transform.quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
                Vec3 horizontal_velocity = this->current_velocity - glm::dot(this->current_velocity, up) * up;

                if (is_zero(move_vector_3d)) {
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
            this->current_velocity = desired_velocity;

            // Get back to the surface because you should not walk away from the planet while grounded
            PlanetInfo* planet = this->grounded_to.value();
            Vec3 planet_position = planet->get_vobject()->transform().get_position();
            Vec3 direction_from_planet = glm::normalize(transform.get_position() - planet_position);
            transform.position() = planet_position + direction_from_planet * planet->get_radius();
        }
    }

    void PlayerController::update_released_camera() {
        Transform& cam_transform = this->camera->get_vobject()->transform();
        Quaternion& cam_quaternion = cam_transform.quaternion();

        PlayerController::SphericalInput spherical = this->get_spherical_input();
        cam_quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));
        cam_quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi));
        cam_quaternion.normalize();

        Vec3 front_of_camera = cam_quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_camera = cam_quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        cam_transform.position() += this->released_camera_speed * this->move_vector_2d.y * front_of_camera;
        cam_transform.position() += this->released_camera_speed * this->move_vector_2d.x * right_of_camera;
    }

    PlayerController::SphericalInput PlayerController::get_spherical_input() {
        PlayerController::SphericalInput spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

    void PlayerController::set_camera_phi(float new_phi) {
        this->camera_phi = std::clamp(new_phi, this->phi_min, this->phi_max); // Don't let the player break their neck
        
        auto& cam_transform = this->camera
            ->get_vobject()
            ->transform();

        cam_transform.quaternion() = Quaternion::from_x_rotation(this->camera_phi);
        cam_transform.quaternion().normalize();
    }

    Vec3 PlayerController::compute_equivalent_gravity() {
        Vec3 gravity_sum(0.0f);
        for (PlanetInfo* planet : this->planets) {
            Vec3 vec_to_planet = planet->get_vobject()->transform().get_position() - this->get_vobject()->transform().get_position();
            float distance_to_planet = glm::length(vec_to_planet);
            if (distance_to_planet > 1e-6f) {
                Vec3 grav_direction = glm::normalize(vec_to_planet);
                gravity_sum += (planet->get_gravity_mass() / distance_to_planet) * grav_direction; // Linear gravity falloff (vector)
            }
        }

        return gravity_sum * PlayerController::GRAVITATIONAL_CONSTANT;
    }

    void PlayerController::jump() {
        // Can only jump if grounded
        if (this->grounded_to.has_value()) {
            this->current_velocity +=
                this->jump_strength * this->get_vobject()->transform().quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
            
            this->grounded_to = std::nullopt;
        }
    }

    void PlayerController::toggle_camera_release() {
        this->released_camera = !this->released_camera;
        auto camera_vobj = this->camera->get_vobject();
        auto& cam_transf = camera_vobj->transform();

        if (this->released_camera) {
            Transform& player_transform = this->get_vobject()->transform(); 

            this->get_vobject()->disown_child(camera_vobj);
            this->stored_child_cam_transform = cam_transf;

            // Keep transform after disowning
            cam_transf.copy_values_from(player_transform);
            cam_transf.position() += cam_transf.quaternion().rotate(this->stored_child_cam_transform.get_position());
        } else {
            this->get_vobject()->add_child(this->camera->get_vobject());
            cam_transf.copy_values_from(this->stored_child_cam_transform);
        }
    }

    void PlayerController::correct_planet_collision() {
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
                /* Collision detected. Correcting... */

                // Remove velocity component in the direction of the planet
                Vec3 direction_to_planet = glm::normalize(vec_to_planet);
                Vec3 velocity_to_planet = glm::dot(this->current_velocity, direction_to_planet) * direction_to_planet;

                // But only remove if moving towards the planet
                if (glm::length(velocity_to_planet) > 0.0f) {
                    this->current_velocity -= velocity_to_planet;
                }

                // Set position to surface
                transform.position() = planet_position - direction_to_planet * planet->get_radius();
                this->grounded_to = planet;

                return; // Please don't collide with multiple planets at once
            }
        }
    }

    void PlayerController::align_to_closest_planet() {
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quaternion = transform.quaternion();
        Vec3 current_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));

        // If grounded, snap to grounded planet normal
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

        { // Align to closest planet
            Vec3 closest_planet_position = this->closest_planet->get_vobject()->transform().get_position();
            Vec3 vec_to_closest_planet = closest_planet_position - transform.get_position();
            Vec3 closest_surface_point = closest_planet_position - glm::normalize(vec_to_closest_planet) * this->closest_planet->get_radius();
            Vec3 vec_to_closest_point = closest_surface_point - transform.get_position();
            float closest_point_distance = glm::length(vec_to_closest_point);

            // If too far from the planet, don't align at all
            if (closest_point_distance > PlayerController::MAX_SURFACE_ALIGNMENT_DISTANCE) {
                return;
            }

            Quaternion align_quat = Quaternion::from_unit_vectors(current_up, -glm::normalize(vec_to_closest_planet));

            // If very close to the planet's surface, snap to planet normal as if grounded
            if (closest_point_distance < PlayerController::MIN_SURFACE_ALIGNMENT_DISTANCE) {
                quaternion.global_compose(align_quat);
                quaternion.normalize();
                return;
            }

            // Otherwise, align partially based on distance to surface
            float planet_alignment_force = 
                (PlayerController::MAX_SURFACE_ALIGNMENT_DISTANCE - closest_point_distance) / // This is OK because closest_point_distance can't be grater here
                    (closest_point_distance - PlayerController::MIN_SURFACE_ALIGNMENT_DISTANCE);

            float frame_planet_alignment = planet_alignment_force * EngineController::get_delta_time();

            Quaternion partial_align_quat = Quaternion::slerp(Quaternion::identity(), align_quat, frame_planet_alignment);
            quaternion.global_compose(partial_align_quat);
            quaternion.normalize();
        }
    }
}
