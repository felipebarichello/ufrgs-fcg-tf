#include "PlayerController.hpp"
#include <engine>

using engine::EngineController;
using engine::Vec3;

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

        Vec3 front_of_player = quaternion.rotate(Vec3(0.0f, 0.0f, -1.0f));
        Vec3 right_of_player = quaternion.rotate(Vec3(1.0f, 0.0f, 0.0f));

        transform.position() += this->speed * this->move_vector.y * front_of_player;
        transform.position() += this->speed * this->move_vector.x * right_of_player;
    }

    void PlayerController::update_direction() {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        glm::vec2 cursor_delta = EngineController::get_input()->get_cursor_position_delta();

        // Atualizamos parâmetros da câmera com os deslocamentos
        this->camera_theta -= this->sensitivity * cursor_delta.x;
        this->camera_phi   -= this->sensitivity * cursor_delta.y;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (this->camera_phi > phimax)
            this->camera_phi = phimax;

        if (this->camera_phi < phimin)
            this->camera_phi = phimin;
    }
}
