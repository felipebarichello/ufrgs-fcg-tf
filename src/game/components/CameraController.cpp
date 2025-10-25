#include "CameraController.hpp"
#include <engine/EngineController.hpp>
#include <cmath>

using namespace game::components;

CameraController::CameraController(PlayerController* player_controller, engine::Camera* camera) {
    this->player_controller = player_controller;
    this->camera = camera;
}

Vec3 lerp(const Vec3& start, const Vec3& end, float t) {
    return start * (1.0f - t) + end * t;
}

void CameraController::Update() {

    auto& player_transform = this->player_controller->get_vobject()->transform();
    Vec3 player_position = player_transform.get_position();

    auto& cam_transform = this->camera->get_vobject()->transform();

    engine::math::Quaternion player_q = player_transform.get_quaternion();
    Vec3 world_offset = player_q.rotate(this->offset);
    //cam_transform.position() = player_position + world_offset;
    cam_transform.position() = lerp(cam_transform.position(), player_position + world_offset, this->camera_smooth_speed);
    //cam_transform.quaternion() = player_q;
    cam_transform.quaternion() = engine::math::Quaternion::slerp(cam_transform.quaternion(), player_q, this->camera_smooth_speed);
}
