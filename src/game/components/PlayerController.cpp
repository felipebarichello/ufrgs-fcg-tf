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
        this->update_transform_due_to_environment();

        if (this->released_camera) {
            this->update_released_camera();
        } else {
            this->update_transform_due_to_input();
        }
    }

    void PlayerController::update_transform_due_to_environment() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();


        /* Position change caused by gravity */

        // TODO: This gravity assumes the planet is flat

        const float gravitational_constant = 9.8f;

        // Calculate (vector) gravity sum from all planets.
        // Each planet contributes (mass / distance) * direction_to_planet.
        Vec3 gravity_sum {0.0f, 0.0f, 0.0f};
        for (PlanetInfo* planet : this->planets) {
            Vec3 vec_to_planet = planet->get_vobject()->transform().get_position() - transform.get_position();
            float distance_to_planet = glm::length(vec_to_planet);
            if (distance_to_planet > 1e-6f) {
                Vec3 grav_direction = glm::normalize(vec_to_planet);
                gravity_sum += (planet->get_gravity_mass() / distance_to_planet) * grav_direction; // Linear gravity falloff (vector)
            }
            std::cout << "REAL: Gravity sum: " << glm::length(gravity_sum) << std::endl;
        }

        Vec3 equivalent_gravity = gravitational_constant * gravity_sum;
        this->current_velocity += equivalent_gravity * EngineController::get_delta_time();

        Vec3 new_position = transform.get_position() + this->current_velocity;
        Vec3 new_vec_to_planet = -new_position; // Planet is at origin
        float distance_to_planet = glm::length(new_vec_to_planet);

        if (distance_to_planet < 200.0f) {
            this->current_velocity = Vec3(0.0f);
        } else {
            transform.position() = new_position;
        }


        /* Direction change due to gravity */
        
        Vec3 up_direction = -glm::normalize(equivalent_gravity);
        Vec3 current_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));

        Quaternion align_quat = Quaternion::from_unit_vectors(current_up, up_direction);

        // Apply only portion of the required alignment each update (smooth/small-step rotation).

        // The closer to the planet, the stronger the alignment
        // float point_of_full_alignment = distance_to_planet - this->planet_radius;
        // float clamped_distance_to_surface = std::max(0.0f, distance_to_planet - this->planet_radius);
        // float alignment_decay = 0.5f;
        // float portion_of_alignment = point_of_full_alignment / (1.0f + clamped_distance_to_surface * alignment_decay);
        // Vec3 full_axis;
        // double full_angle = align_quat.from_axis_angle(full_axis);
        // if (std::abs(full_angle) > 1e-12) {
        //     double step_angle = full_angle * portion_of_alignment;
        //     align_quat = Quaternion::from_axis_angle(full_axis, step_angle);
        //     align_quat.normalize_inplace();
        // }

        quaternion.global_compose(align_quat);
        quaternion.normalize();
    }

    void PlayerController::update_transform_due_to_input() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();


        /* Camera (attached) movement */

        PlayerController::SphericalInput spherical = this->get_spherical_input();
        this->set_camera_phi(this->camera_phi + spherical.delta_phi);
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));


        /* Walking movement */

        // TODO: Should not be able to walk while not touching ground

        Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        transform.position() += this->speed * this->move_vector.y * front_of_player;
        transform.position() += this->speed * this->move_vector.x * right_of_player;
    }

    void PlayerController::update_released_camera() {
        auto& cam_transform = this->camera->get_vobject()->transform();
        auto& cam_quaternion = cam_transform.quaternion();

        PlayerController::SphericalInput spherical = this->get_spherical_input();
        cam_quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));
        cam_quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi));
        cam_quaternion.normalize();

        Vec3 front_of_player = cam_quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = cam_quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        cam_transform.position() += this->speed * this->move_vector.y * front_of_player;
        cam_transform.position() += this->speed * this->move_vector.x * right_of_player;
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

    void PlayerController::jump() {
        this->current_velocity +=
            this->jump_strength * this->get_vobject()->transform().quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
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
            cam_transf = this->stored_child_cam_transform;
        }
    }
}
