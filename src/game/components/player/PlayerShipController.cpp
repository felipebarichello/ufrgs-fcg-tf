#include "PlayerShipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <glm/vec3.hpp>
#include <cmath>
#include <game/scenes/MainScene_vars.hpp>

using namespace engine;

namespace game::components {

    void PlayerShipController::Awake() {
        this->ship_command = &this->ship_controller->get_command();
        this->camera_controller->link_thrusting(&this->ship_command->thrusting, &this->ship_controller->fuel);

        // Physics setup
        this->kinematic = this->ship_controller->get_kinematic_body();
        this->angular = this->ship_controller->get_angular_velocity();
    }

    void PlayerShipController::Start() {
        // Input setup

        ShipCommand& cmd = this->ship_command_buffer;
        InputController* input = EngineController::get_input();

        input->subscribe_hold_button(GLFW_KEY_W, &cmd.thrusting);
        input->subscribe_hold_button(GLFW_KEY_A, &cmd.rolling_left);
        input->subscribe_hold_button(GLFW_KEY_D, &cmd.rolling_right);
    }

    void PlayerShipController::Update() {
        this->ship_command->thrusting     = this->ship_command_buffer.thrusting;
        this->ship_command->rolling_left  = this->ship_command_buffer.rolling_left;
        this->ship_command->rolling_right = this->ship_command_buffer.rolling_right;
        this->ship_command->steer = this->get_smooth_spherical_input();
    }

    SphericalCoords PlayerShipController::get_smooth_spherical_input() {
        SphericalCoords spherical = this->get_spherical_input();
        spherical.delta_theta = std::lerp(this->smooth_spherical_input.delta_theta, spherical.delta_theta, theta_lerp);
        spherical.delta_phi   = std::lerp(this->smooth_spherical_input.delta_phi,   spherical.delta_phi,   phi_lerp);
        this->smooth_spherical_input = spherical;
        return spherical;
    }

    SphericalCoords PlayerShipController::get_spherical_input() {
        SphericalCoords spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

    void PlayerShipController::OnEnable() {
        this->model->set_visible();
        this->ship_controller->enable();
        this->camera_controller->enable();
    }

    void PlayerShipController::OnDisable() {
        this->model->set_invisible();
        this->camera_controller->disable();
        this->ship_controller->disable();
    }

    bool PlayerShipController::collided_with_planets() {
        for (PlanetInfo* planet : this->planets) {
            bool collision_detected = engine::collision::collide_capsule_sphere(*this->capsule_collider, *planet->get_sphere_collider()).has_collided();
            if (collision_detected && this->is_enabled()) {
                return true;
            }
        }
        return false;
    }
}