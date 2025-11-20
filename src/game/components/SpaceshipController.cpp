#include "SpaceshipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>

using engine::EngineController;
using engine::InputController;
using engine::Vec2;
using engine::Vec3;
using engine::math::Quaternion;

namespace game::components {
    struct SpaceshipController::SphericalInput {
        float delta_theta;
        float delta_phi;
    };

    void SpaceshipController::Start() {
        InputController* input = EngineController::get_input();
        input->subscribe_dpad(&this->move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        input->subscribe_hold_button(GLFW_KEY_W, &this->accelerating_forward);
        input->subscribe_hold_button(GLFW_KEY_S, &this->accelerating_backward);
    }

    void SpaceshipController::Update() {
        // Inertial spaceship: integrate acceleration -> velocity -> position
        Vec3 forward = this->get_vobject()->transform().get_quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));

        float dt = EngineController::get_delta_time();

        // Build acceleration vector from input
        Vec3 accel_vec(0.0f);
        if (this->accelerating_forward) accel_vec += forward * thrust_power;
        if (this->accelerating_backward) accel_vec -= forward * thrust_power;

        // Integrate velocity
        this->current_velocity += accel_vec * dt;

        // Integrate position
        this->get_vobject()->transform().position() += this->current_velocity * dt;

        // Update rotation from input and camera
        this->update_rotation_due_to_input();
    }

    void SpaceshipController::update_rotation_due_to_input() {
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quaternion = transform.quaternion();

        /* Camera (attached) movement */

        SpaceshipController::SphericalInput spherical = this->get_spherical_input();
        float dt = EngineController::get_delta_time();
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta * dt * 50.0f));
        quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi * dt * 50.0f));
        quaternion.local_compose(Quaternion::from_z_rotation(-this->move_vector.x * dt * 5.0f));
        quaternion.normalize();
    }

    SpaceshipController::SphericalInput SpaceshipController::get_spherical_input() {
        SpaceshipController::SphericalInput spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

}
