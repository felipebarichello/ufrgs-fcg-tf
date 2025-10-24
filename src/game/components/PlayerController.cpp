#include "PlayerController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>

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
        // Use a press to perform a single jump impulse instead of holding space.
        input->subscribe_press_button(GLFW_KEY_SPACE, std::bind(&PlayerController::jump, this));
    }

    void PlayerController::Update() {

        float dt = EngineController::get_delta_time();

        Vec3 gravity_accel = this->compute_gravity_accel();
        Vec3 pos = this->get_vobject()->transform().get_position();
        Vec3 up = -glm::normalize(pos);
        Vec3 input_vel = this->compute_input_velocity();

        if (grounded) {
            float vertical_component = glm::dot(this->current_velocity, up);
            this->current_velocity = input_vel + up * vertical_component;
        } else {
            // TODO: it is not phyisically correct
            this->current_velocity += input_vel * dt;
        }

        this->current_velocity += gravity_accel * dt;
        this->update_transform();
    }


    Vec3 PlayerController::compute_gravity_accel() {
        auto& transform = this->get_vobject()->transform();

            /* Position change caused by gravity */

            // TODO: This gravity assumes the planet is flat

        const float gravity_accel = 30.0f; // approximate gravity magnitude (units/sec^2)

        Vec3 vec_to_planet = -transform.get_position(); // Planet is at origin
        Vec3 planet_direction = glm::normalize(vec_to_planet);

        Vec3 gravity_direction = planet_direction; // direction towards center
        // Integrate gravity using dt: v += a * dt
        return gravity_accel * gravity_direction;
    }

    void PlayerController::align_quaternion_to_planet_surface() {

        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();

        Vec3 vec_to_planet = -transform.get_position(); // Planet is at origin
        Vec3 planet_direction = glm::normalize(vec_to_planet);

        Vec3 up_direction = -planet_direction;
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

    Vec3 PlayerController::compute_input_velocity() {
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

        return  this->speed * this->move_vector.y * front_of_player +
                this->speed * this->move_vector.x * right_of_player;
    }

    void PlayerController::update_transform() {
        auto& transform = this->get_vobject()->transform();
        float dt = EngineController::get_delta_time();
        Vec3 new_position = transform.position() + this->current_velocity * dt;

        if (h_norm(new_position) < this->planet_radius) {
            this->grounded = true;
            // Prevent going inside the planet: clamp to surface
            Vec3 planet_direction = glm::normalize(new_position);
            transform.position() = planet_direction * this->planet_radius;
            // Remove any inward velocity component so we don't immediately
            // re-enter the planet on the next frame.
            float velocity_towards_center = glm::dot(this->current_velocity, -planet_direction);
            if (velocity_towards_center > 0.0f) {
                this->current_velocity -= velocity_towards_center * -planet_direction;
            }
        } else {
            transform.position() = new_position;
            this->grounded = false;
        }

        this->align_quaternion_to_planet_surface();
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

    void PlayerController::jump() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();
        Vec3 player_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));
        // Store impulse as a velocity change (instant)
        this->jump_impulse = this->jump_strength * player_up; // units: m/s
        this->current_velocity += this->jump_impulse;
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
