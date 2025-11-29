#include "ShipCameraController.hpp"
#include <engine/EngineController.hpp>
#include <engine/math/linalg.hpp>
#include <cmath>

using namespace game::components;
using engine::Vec3;
using engine::Quaternion;
using engine::Transform;

void ShipCameraController::PostUpdate() {
    Transform& ship_transform = this->get_vobject()->transform();
    Quaternion ship_quat = ship_transform.get_local_quaternion();
    Transform& cam_transform = this->camera->get_vobject()->transform();

    // Smoothly rotate camera to match ship orientation
    Quaternion new_cam_quat = Quaternion::slerp(cam_transform.local_quaternion(), ship_quat, this->camera_rotation_smooth);

    Vec3 cam_offset = Vec3(0.0f, this->vertical_offset, this->default_distance);

    /* Handle animation if applicable */

    if (this->is_animating) {
        float dt = engine::EngineController::get_delta_time();
        this->animation_timer += dt;

        if (this->animation_timer >= this->animation_time) {
            this->is_animating = false;
        } else {
            float t = this->animation_timer / this->animation_time;
            float eased_t = engine::ease_out_quad(t);

            Vec3 point_2 = Vec3(
                0.0f,
                this->animation_bezier_2.x,
                this->animation_bezier_2.y
            );

            Vec3 point_3 = Vec3(
                0.0f,
                this->animation_bezier_3.x,
                this->animation_bezier_3.y
            );

            cam_offset = engine::bezier3(
                Vec3(0.0f, 0.0f, 0.0f),
                point_2,
                point_3,
                cam_offset,
                eased_t
            );
        }
    }
    
    /* Update transform */

    cam_offset = new_cam_quat.rotate(cam_offset);
    cam_transform.set_world_position(ship_transform.get_world_position() + cam_offset);
    cam_transform.local_quaternion() = new_cam_quat;
}

void ShipCameraController::OnEnable() {
    this->animation_timer = 0.0f;
    this->is_animating = true;

    // Set ship camera to humanoid camera position
    Transform& human_cam_transf = this->humanoid_camera->get_vobject()->transform();
    Transform& ship_cam_transf = this->camera->get_vobject()->transform();
    ship_cam_transf.set_world_position(human_cam_transf.get_world_position());
    ship_cam_transf.set_world_quaternion(human_cam_transf.get_world_quaternion());
}
