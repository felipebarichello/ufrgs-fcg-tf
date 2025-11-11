#include "SpaceshipPlayerController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>

using engine::EngineController;
using engine::InputController;
using engine::Vec2;
using engine::Vec3;
using engine::math::Quaternion;

namespace game::components {
    struct SpaceshipPlayerController::SphericalInput {
        float delta_theta;
        float delta_phi;
    };

    void SpaceshipPlayerController::Start() {
        InputController* input = EngineController::get_input();
        input->subscribe_dpad(&this->move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        input->subscribe_hold_button(GLFW_KEY_W, &this->accelerating_forward);
        input->subscribe_hold_button(GLFW_KEY_S, &this->accelerating_backward);
    }

    void SpaceshipPlayerController::Update() {
        //this->update_transform_due_to_environment();
        Vec3 player_forward = this->get_vobject()->transform().quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        // Update player position
        if (this->accelerating_forward) {
            this->acceleration = 1.0f;
        } else if (this->accelerating_backward) {
            this->acceleration = -1.0f;
        } else {
            this->acceleration = 0.0f;
        }

        float delta_time = EngineController::get_delta_time();    

        this->speed += this->acceleration * delta_time * 100.0f;
        if (this->speed > this->max_speed) this->speed = this->max_speed;
        if (this->speed < this->min_speed) this->speed = this->min_speed;
        this->get_vobject()->transform().position() += this->speed * delta_time * player_forward;

        this->update_transform_due_to_input();
        this->update_camera();
    }

    void SpaceshipPlayerController::update_transform_due_to_input() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();

        /* Camera (attached) movement */

        SpaceshipPlayerController::SphericalInput spherical = this->get_spherical_input();
        // Make rotations frame-rate independent by scaling with delta time
        float dt = EngineController::get_delta_time();
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta * dt * 50.0f));
        quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi * dt * 50.0f));
        quaternion.local_compose(Quaternion::from_z_rotation(-this->move_vector.x * dt * 5.0f));
        quaternion.normalize();
        
    }

    void SpaceshipPlayerController::update_camera() {
        auto& cam_transform = this->camera->get_vobject()->transform();
        cam_transform.position() = Vec3(0.0f, 0.0f, -1.0f);
    }

    SpaceshipPlayerController::SphericalInput SpaceshipPlayerController::get_spherical_input() {
        SpaceshipPlayerController::SphericalInput spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

}
