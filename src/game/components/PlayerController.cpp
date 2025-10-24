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
        // auto& transform = this->get_vobject()->transform();
        // auto& quaternion = transform.quaternion();


        /* Position change caused by gravity */

        // TODO: This gravity assumes the planet is flat
        // const float estimated_frame_period = 1.0f / 60.0f; // TODO: Delta time (note: estimate integration)
        // const float gravity_accel = 4.0f; // TODO: Consider distance to center of mass
        // Vec3 gravity_direction = -glm::normalize(transform.position()); // Planet is at origin
        // this->current_velocity += gravity_accel * estimated_frame_period * gravity_direction;

        // Vec3 new_position = transform.position() + this->current_velocity;
        // Vec3 vec_distance_to_planet = new_position; // Planet is at origin
        // float distance_to_planet = glm::length(vec_distance_to_planet);

        // if (distance_to_planet < this->planet_radius) {
        //     this->current_velocity = Vec3(0.0f);
        // } else {
        //     transform.position() = new_position;
        // }


        /* Direction change due to gravity */

        // Vec3 vec_distance_to_planet = transform.position(); // Planet is at origin
        // Vec3 up_direction = glm::normalize(vec_distance_to_planet);
        // Vec3 current_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));
        // Quaternion align_quat = Quaternion::fromUnitVectors(current_up, up_direction);
        // quaternion *= align_quat;
    }

    void PlayerController::update_transform_due_to_input() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();


        /* Camera (attached) movement */

        PlayerController::SphericalInput spherical = this->get_spherical_input();
        this->set_camera_phi(this->camera_phi + spherical.delta_phi);
        quaternion *= Quaternion::from_y_rotation(spherical.delta_theta);


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
        cam_quaternion *= Quaternion::from_y_rotation(spherical.delta_theta);
        cam_quaternion *= Quaternion::from_x_rotation(spherical.delta_phi);

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
