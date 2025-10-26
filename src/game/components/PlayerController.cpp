#include "PlayerController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include "PlanetInfo.hpp"

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
        input->subscribe_dpad(&this->move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
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
            
            transform.position() += this->speed * this->move_vector.y * front_of_player;
            transform.position() += this->speed * this->move_vector.x * right_of_player;

            // Get back to the surface because you should not get away from the planet while grounded grounded
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

        cam_transform.position() += this->released_camera_speed * this->move_vector.y * front_of_camera;
        cam_transform.position() += this->released_camera_speed * this->move_vector.x * right_of_camera;
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
                std::cout << "Too far" << std::endl;
                return;
            }

            std::cout << "Not too far" << std::endl;

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
