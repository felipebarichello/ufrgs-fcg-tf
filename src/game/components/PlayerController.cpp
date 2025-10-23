#include "PlayerController.hpp"
#include <engine>

using engine::EngineController;
using engine::Vec3;
using engine::math::Quaternion;

namespace game::components {
    void PlayerController::Start() {
        EngineController::get_input()->subscribe_dpad(&this->move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
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
        const float estimated_frame_period = 1.0f / 60.0f; // TODO: Delta time (note: estimate integration)
        const float gravity_accel = 4.0f; // TODO: Consider distance to center of mass
        Vec3 gravity_direction = -glm::normalize(transform.position()); // Planet is already on origin
        this->current_velocity += gravity_accel * estimated_frame_period * gravity_direction;
        transform.position() += this->current_velocity;


        /* Caused by input */

        Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        transform.position() += this->speed * this->move_vector.y * front_of_player;
        transform.position() += this->speed * this->move_vector.x * right_of_player;
    }

    void PlayerController::update_direction() {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        glm::vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

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

        // Rotate player in axis y by delta_theta
        auto& transform = this->get_vobject()->transform();
        transform.quaternion() *= Quaternion::fromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), delta_theta);
    }
}
