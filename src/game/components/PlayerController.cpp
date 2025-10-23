#include "PlayerController.hpp"
#include <engine>
#include <InputController.hpp>

using engine::EngineController;
using engine::InputController;
using engine::Vec2;
using engine::Vec3;
using engine::math::Quaternion;

namespace game::components {
    void PlayerController::Start() {
        InputController* input = EngineController::get_input();
        input->subscribe_dpad(&this->move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        input->subscribe_press_button(GLFW_KEY_F, std::bind(&PlayerController::toggle_camera_release, this));
    }

    void PlayerController::Update() {
        this->update_position();
        this->update_direction();
    }

    void PlayerController::update_position() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();


        /* Caused by gravity */

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


        /* Caused by input */

        Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        transform.position() += this->speed * this->move_vector.y * front_of_player;
        transform.position() += this->speed * this->move_vector.x * right_of_player;
    }

    void PlayerController::update_direction() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();

        
        /* Caused by gravity */

        // Vec3 vec_distance_to_planet = transform.position(); // Planet is at origin
        // Vec3 up_direction = glm::normalize(vec_distance_to_planet);
        // Vec3 current_up = quaternion.rotate(Vec3(0.0f, 1.0f, 0.0f));
        // Quaternion align_quat = Quaternion::fromUnitVectors(current_up, up_direction);
        // quaternion *= align_quat;


        /* Caused by input */

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        float delta_theta = this->h_sensitivity * -cursor_delta.x;
        this->camera_phi -= this->v_sensitivity * cursor_delta.y;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (this->camera_phi > phimax)
            this->camera_phi = phimax;

        if (this->camera_phi < phimin)
            this->camera_phi = phimin;

        quaternion *= Quaternion::fromYRotation(delta_theta);

        auto& cam_quaternion = this->camera
            ->get_vobject()
            ->transform()
            .quaternion();
        
        cam_quaternion = Quaternion::fromXRotation(this->camera_phi);
    }

    void PlayerController::toggle_camera_release() {
        this->released_camera = !this->released_camera;
        auto camera_vobj = this->camera->get_vobject();

        if (this->released_camera) {
            this->get_vobject()->disown_child(camera_vobj);
            this->stored_child_cam_transform = camera_vobj->transform();
            camera_vobj->transform().copy_values_from(this->get_vobject()->transform()); // Keep transform after disowning
        } else {
            this->get_vobject()->add_child(this->camera->get_vobject());
            camera_vobj->transform() = this->stored_child_cam_transform;
        }
    }
}
