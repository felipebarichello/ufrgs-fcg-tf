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

        // Physics setup
        this->kinematic = this->ship_controller->get_kinematic_body();
        this->angular = this->ship_controller->get_angular_velocity();
    }

    void PlayerShipController::Start() {
        // Input setup

        ShipCommand& cmd = *this->ship_command;
        InputController* input = EngineController::get_input();

        input->subscribe_hold_button(GLFW_KEY_W, &cmd.thrusting);
        input->subscribe_hold_button(GLFW_KEY_A, &cmd.rolling_left);
        input->subscribe_hold_button(GLFW_KEY_D, &cmd.rolling_right);
    }

    void PlayerShipController::Update() {
        this->ship_command->steer = this->get_smooth_spherical_input();
    }

    void PlayerShipController::PostUpdate() {
        this->test_planet_collisions();
    }

    SphericalInput PlayerShipController::get_smooth_spherical_input() {
        SphericalInput spherical = this->get_spherical_input();
        spherical.delta_theta = std::lerp(this->smooth_spherical_input.delta_theta, spherical.delta_theta, theta_lerp);
        spherical.delta_phi   = std::lerp(this->smooth_spherical_input.delta_phi,   spherical.delta_phi,   phi_lerp);
        this->smooth_spherical_input = spherical;
        return spherical;
    }

    SphericalInput PlayerShipController::get_spherical_input() {
        SphericalInput spherical;

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
    }

    void PlayerShipController::OnDisable() {
        this->model->set_invisible();
        this->ship_controller->disable();
    }

    void PlayerShipController::test_planet_collisions() {
        for (PlanetInfo* planet : this->planets) {
            bool collision_detected = engine::collision::collide_cylinder_sphere(*this->cylinder_collider, *planet->get_sphere_collider()).has_collided();
            if (collision_detected) {
                scenes::main_scene::game_over_text->setText(std::string("GAME OVER"), 1.8f, engine::Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
            }
        }
    }
}