#include "SpaceshipController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <engine/vobject/components/TextDrawable.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <iomanip>

using namespace engine;

namespace game::components {

    void SpaceshipController::Awake() {
        TextDrawable* f_text = new TextDrawable();
        TextDrawable* go_text = new TextDrawable();
        this->fuel_text = f_text;
        this->game_over_text = go_text;
        this->get_vobject()->add_component(f_text);
        this->get_vobject()->add_component(go_text);
    }

    void SpaceshipController::Start() {
        InputController* input = EngineController::get_input();
        input->subscribe_hold_button(GLFW_KEY_W, &this->thrusting);
        input->subscribe_hold_button(GLFW_KEY_A, &this->rolling_left);
        input->subscribe_hold_button(GLFW_KEY_D, &this->rolling_right);

        // Create text drawable to show fuel on-screen (top-left corner)
        std::ostringstream init_ss;
        init_ss << std::fixed << std::setprecision(1) << this->fuel;
        this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
        this->game_over_text->setText(std::string(""), 3.0f, engine::Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
    }

    void SpaceshipController::Update() {
        Transform& transform = this->get_vobject()->transform();

        // Update rotation from input and camera
        this->update_rotation_due_to_input();

        // Inertial spaceship: integrate acceleration -> velocity -> position
        Vec3 forward = transform.get_quaternion().rotate(Vec3(0.0f, 0.0f, -1.0f));
        
        float dt = EngineController::get_delta_time();
        
        if (this->fuel > 0.0f) {
            // Passive fuel consumption
            this->fuel -= this->passive_fuel_consumption * dt;

            // Thrusting
            if (this->thrusting) {
                this->fuel -= dt * this->thrust_fuel_consumption;
                this->kinematic->set_velocity(this->kinematic->get_velocity() + forward * thrust_power * dt);
            }

            // Rolling
            Quaternion& quaternion = transform.quaternion();
            if (this->rolling_left) {
                this->fuel -= dt * this->roll_fuel_consumption;
                quaternion.local_compose(Quaternion::from_z_rotation(roll_power * dt));
            }
            if (this->rolling_right) {
                this->fuel -= dt * this->roll_fuel_consumption;
                quaternion.local_compose(Quaternion::from_z_rotation(-roll_power * dt));
            }
        }

        // Update on-screen fuel text
        if (this->fuel_text) {
            std::ostringstream init_ss;
            init_ss << std::fixed << std::setprecision(1) << this->fuel;
            this->fuel_text->setText(std::string("Fuel: ") + init_ss.str(), 1.5f, engine::Vec3(1.0f), -0.95f, 0.9f);
        }
    }

    void SpaceshipController::PostUpdate() {
        this->test_planet_collisions();
    }

    void SpaceshipController::update_rotation_due_to_input() {
        Transform& transform = this->get_vobject()->transform();
        Quaternion& quaternion = transform.quaternion();
        float dt = EngineController::get_delta_time();

        SphericalInput spherical = this->get_spherical_input();
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta * dt * 50.0f));
        quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi * dt * 50.0f));
        quaternion.normalize();
    }

    SphericalInput SpaceshipController::get_spherical_input() {
        SphericalInput spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

    void SpaceshipController::OnEnable() {
        this->model->set_visible();
    }

    void SpaceshipController::OnDisable() {
        this->model->set_invisible();
    }

    void SpaceshipController::test_planet_collisions() {
        for (PlanetInfo* planet : this->planets) {
            bool collision_detected = engine::collision::collide_cylinder_sphere(*this->cylinder_collider, *planet->get_sphere_collider()).has_collided();
            if (collision_detected) {
                this->game_over_text->setText(std::string("GAME OVER"), 1.8f, engine::Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
            }
        }
    }
}