#include "SpaceshipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <iomanip>

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

        // Create text drawable to show fuel on-screen (top-left corner)
        this->fuel_text = new TextDrawable();
        std::ostringstream init_ss;
        init_ss << std::fixed << std::setprecision(1) << this->fuel;
        this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, glm::vec3(1.0f), -0.95f, 0.9f);
    }

    void SpaceshipController::Update() {
        if (!this->active) return;
        
        // Inertial spaceship: integrate acceleration -> velocity -> position
        Vec3 forward = this->get_vobject()->transform().get_quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));

        float dt = EngineController::get_delta_time();

        // Build acceleration vector from input
        float accel_dir = 0.0f;
        bool thrusting = this->accelerating_forward || this->accelerating_backward;
        if (thrusting && this->fuel > 0.0f) {
            this->fuel -= dt * this->fuel_consumption_rate; // Consume fuel
            if (this->accelerating_forward)  accel_dir += thrust_power;
            if (this->accelerating_backward) accel_dir -= thrust_power;
        }

        Vec3 accel_vec = forward * accel_dir;

        // Integrate velocity
        this->current_velocity += accel_vec * dt;

        // Integrate position
        this->get_vobject()->transform().position() += this->current_velocity * dt;

        // Update on-screen fuel text
        if (this->fuel_text) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1) << std::max(0.0f, this->fuel);
            this->fuel_text->setText(std::string("Fuel: ") + ss.str(), 1.8f, glm::vec3(1.0f), -0.95f, 0.95f);
        }

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

    void SpaceshipController::enable() {
        this->active = true;
        this->model->set_visible();
    }

    void SpaceshipController::disable() {
        this->active = false;
        this->model->set_invisible();
    }

}