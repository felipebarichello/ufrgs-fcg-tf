#include "SpaceshipCameraController.hpp"
#include <engine/EngineController.hpp>
#include <engine/math/linalg.hpp>
#include <cmath>

using namespace game::components;
using engine::Vec3;
using engine::Quaternion;
using engine::Transform;

void SpaceshipCameraController::Update() {
    Transform& ship_transform = this->ship_controller->get_vobject()->transform();
    Quaternion ship_quat = ship_transform.get_quaternion();
    Transform& cam_transform = this->camera->get_vobject()->transform();
    Vec3 up_dir = ship_quat.rotate(Vec3(0.0f, 1.0f, 0.0f));

    Quaternion new_cam_quat = Quaternion::slerp(cam_transform.quaternion(), ship_quat, this->camera_rotation_smooth);
    Vec3 inert_cam_offset = new_cam_quat.rotate(Vec3(0.0f, 0.0f, this->default_distance));
    Vec3 total_cam_offset = inert_cam_offset + up_dir * this->vertical_offset;
    
    cam_transform.set_world_position(ship_transform.get_world_position() + total_cam_offset);
    cam_transform.quaternion() = new_cam_quat;
}
