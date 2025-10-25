#include "CameraController.hpp"
#include <engine/EngineController.hpp>
#include <cmath>

using namespace game::components;

CameraController::CameraController(PlayerController* player_controller, engine::Camera* camera) {
    this->player_controller = player_controller;
    this->camera = camera;
}

void CameraController::Update() {
    // Defensive checks
    if (!this->player_controller) return;
    if (!this->player_controller->get_vobject()) return;
    if (!this->camera) return;
    if (!this->camera->get_vobject()) return;

    auto& player_transform = this->player_controller->get_vobject()->transform();
    Vec3 player_position = player_transform.get_position();

    auto& cam_transform = this->camera->get_vobject()->transform();

    engine::math::Quaternion player_q = player_transform.get_quaternion();
    Vec3 world_offset = player_q.rotate(this->offset);
    cam_transform.position() = player_position + world_offset;
    cam_transform.quaternion() = player_q;
}