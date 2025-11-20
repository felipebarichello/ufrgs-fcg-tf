#include "SpaceshipCameraController.hpp"
#include <engine/EngineController.hpp>
#include <engine/math/linalg.hpp>
#include <cmath>

using namespace game::components;

SpaceshipCameraController::SpaceshipCameraController(SpaceshipController* player_controller, engine::Camera* camera) {
    this->player_controller = player_controller;
    this->camera = camera;
}

void SpaceshipCameraController::Update() {
    Transform& player_transform = this->player_controller->get_vobject()->transform();
    Vec3 player_position = player_transform.get_position();
    Transform& cam_transform = this->camera->get_vobject()->transform();

    engine::math::Quaternion player_q = player_transform.get_quaternion();
    Vec3 world_offset = player_q.rotate(this->offset);

    cam_transform.position() = engine::lerp(cam_transform.position(), player_position + world_offset, this->camera_smooth_speed);
    cam_transform.quaternion() = engine::math::Quaternion::slerp(cam_transform.quaternion(), player_q, this->camera_smooth_speed);
}
