#include "HumanoidPlayerController.hpp"
#include <engine>
#include <InputController.hpp>
#include <algorithm>
#include <game/components/PlanetInfo.hpp>
#include <cstdio>
#include <game/components/WalkerController.hpp>

using engine::EngineController;
using engine::InputController;
using engine::Vec2;
using engine::Vec3;
using engine::math::Quaternion;
using engine::is_zero;
using engine::Transform;
using engine::to_string;

namespace game::components {
    struct HumanoidPlayerController::SphericalInput {
        float delta_theta;
        float delta_phi;
    };

    void HumanoidPlayerController::Start() {
        InputController* input = EngineController::get_input();
        // Subscribe input into local move vector and forward it to WalkerController in Update
        input->subscribe_dpad(&this->move_vector_2d, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        input->subscribe_press_button(GLFW_KEY_F6, std::bind(&HumanoidPlayerController::toggle_camera_release, this));
        // Jump now forwarded to WalkerController
        input->subscribe_press_button(GLFW_KEY_SPACE, std::bind(&HumanoidPlayerController::on_jump_pressed, this));

        // Capture the initial local transform of the child camera (used as base for bobbing offsets)
        if (this->camera && this->camera->get_vobject()) {
            this->stored_child_cam_transform = this->camera->get_vobject()->transform();
        }
    }

    void HumanoidPlayerController::Update() {
        if (this->active) {
            if (this->released_camera) {
                this->update_released_camera();
            } else {
                this->update_transform_due_to_input();
                
                // Forward input to walker every frame
                if (this->walker) {
                    this->walker->set_move_vector(this->move_vector_2d);
                }
            }
        }
    }

    // Movement/environment handled by WalkerController. Humanoid keeps camera and input only.

    void HumanoidPlayerController::update_transform_due_to_input() {
        auto& transform = this->get_vobject()->transform();
        auto& quaternion = transform.quaternion();

        /* Camera (attached) movement */
        HumanoidPlayerController::SphericalInput spherical = this->get_spherical_input();
        this->set_camera_phi(this->camera_phi + spherical.delta_phi);
        quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));

        // Apply view bobbing to the child camera when attached
        if (!this->released_camera && this->camera && this->walker) {
            // Use input magnitude as a proxy for movement intensity
            engine::Vec2 mv = this->walker->get_move_vector();
            float input_mag = glm::length(mv);

            float dt = EngineController::get_delta_time();
            if (input_mag > this->bob_min_input_threshold) {
                // Advance bob timer faster when moving more
                this->bob_timer += dt * (this->bob_frequency * (0.5f + input_mag));
            } else {
                // Slowly decay bob timer when stopped to make bobbing stop smoothly
                this->bob_timer += dt * (this->bob_frequency * 0.2f);
            }

            float bob_y = sinf(this->bob_timer * 2.0f) * this->bob_vertical_amplitude * input_mag;
            float bob_x = sinf(this->bob_timer) * this->bob_sway_amplitude * input_mag;

            // Apply offset in local camera space based on stored child transform
            Transform& cam_transform = this->camera->get_vobject()->transform();
            // Preserve camera quaternion (pitch) set by set_camera_phi; only reset local position to stored base
            cam_transform.position() = this->stored_child_cam_transform.get_position();
            cam_transform.position() += cam_transform.quaternion().rotate(Vec3(bob_x, bob_y, 0.0f));
        }

        // Forwarded movement input is handled by WalkerController; Humanoid doesn't change position directly.
    }

    void HumanoidPlayerController::update_released_camera() {
        Transform& cam_transform = this->camera->get_vobject()->transform();
        Quaternion& cam_quaternion = cam_transform.quaternion();

        HumanoidPlayerController::SphericalInput spherical = this->get_spherical_input();
        cam_quaternion.local_compose(Quaternion::from_y_rotation(spherical.delta_theta));
        cam_quaternion.local_compose(Quaternion::from_x_rotation(spherical.delta_phi));
        cam_quaternion.normalize();

        Vec3 front_of_camera = cam_quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_camera = cam_quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        cam_transform.position() += this->released_camera_speed * this->move_vector_2d.y * front_of_camera;
        cam_transform.position() += this->released_camera_speed * this->move_vector_2d.x * right_of_camera;
    }

    HumanoidPlayerController::SphericalInput HumanoidPlayerController::get_spherical_input() {
        HumanoidPlayerController::SphericalInput spherical;

        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        Vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        spherical.delta_theta = this->h_sensitivity * -cursor_delta.x;
        spherical.delta_phi = this->v_sensitivity * -cursor_delta.y;

        return spherical;
    }

    void HumanoidPlayerController::set_camera_phi(float new_phi) {
        this->camera_phi = std::clamp(new_phi, this->phi_min, this->phi_max); // Don't let the player break their neck
        
        auto& cam_transform = this->camera
            ->get_vobject()
            ->transform();

        cam_transform.quaternion() = Quaternion::from_x_rotation(this->camera_phi);
        cam_transform.quaternion().normalize();
    }

    void HumanoidPlayerController::on_jump_pressed() {
        if (this->walker) this->walker->request_jump();
    }

    void HumanoidPlayerController::toggle_camera_release() {
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
            cam_transf.copy_values_from(this->stored_child_cam_transform);
        }
    }

    // Humanoid no longer handles gravity/collision/alignment. That is WalkerController's responsibility.
}
