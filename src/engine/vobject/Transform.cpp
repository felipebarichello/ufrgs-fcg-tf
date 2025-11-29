#include "Transform.hpp"
#include "VObject.hpp"
// #include <engine/math/linalg.hpp>

using namespace engine;

void Transform::set_world_position(const Vec3& new_pos) {
    // std::cout << "new pos: " << new_pos.to_string() << std::endl;
    auto parent_opt = this->get_parent();

    if (parent_opt) {
        Transform* parent_transform = parent_opt.value();
        Mat4 parent_model = parent_transform->get_model_matrix();
        Mat4 parent_inverse = invert_orthonormal_matrix(parent_model);
        Vec4 new_world_pos = Vec4(new_pos, 1.0f);
        
        // Convert world position to local position
        Vec4 local_pos_homogeneous = parent_inverse * new_world_pos;
        this->_local_position = Vec3(local_pos_homogeneous.x, local_pos_homogeneous.y, local_pos_homogeneous.z);
    } else {
        // No parent, so local position is the same as world position
        this->_local_position = new_pos;
        // std::cout << "Which is trivial" << std::endl;
    }

    // std::cout << "local pos set to: " << this->_local_position.to_string() << std::endl;

    this->dirty = true;
}

Quaternion Transform::get_world_quaternion() {
    auto parent_opt = this->get_parent();
    if (parent_opt) {
        Transform* parent_transform = parent_opt.value();
        Quaternion parent_world_quat = parent_transform->get_world_quaternion();
        return parent_world_quat * this->_quaternion;
    } else {
        return this->_quaternion;
    }
}

void Transform::set_world_quaternion(const Quaternion& quat) {
    auto parent_opt = this->get_parent();
    if (parent_opt) {
        Transform* parent_transform = parent_opt.value();
        Quaternion parent_world_quat = parent_transform->get_world_quaternion();
        Quaternion parent_world_conjugate = parent_world_quat.conjugated();
        this->_quaternion = parent_world_conjugate * quat;
    } else {
        this->_quaternion = quat;
    }

    this->dirty = true;
}

Mat4 Transform::get_model_matrix() {
    if (this->dirty) {
        this->update_matrix();
    }

    auto p = this->get_parent();
    if (p) {
        return p.value()->get_model_matrix() * this->transform_matrix;
    } else {
        return this->transform_matrix;
    }
}

void Transform::update_matrix() {
    // Build rotation matrix from quaternion
    float w = static_cast<float>(this->_quaternion.w());
    Vec3 v = this->_quaternion.v();
    float x = v.x, y = v.y, z = v.z;

    float ww = w*w;
    float xx = x*x;
    float yy = y*y;
    float zz = z*z;

    float m00 = ww + xx - yy - zz;
    float m01 = 2.0f * (x*y - w*z);
    float m02 = 2.0f * (x*z + w*y);

    float m10 = 2.0f * (x*y + w*z);
    float m11 = ww - xx + yy - zz;
    float m12 = 2.0f * (y*z - w*x);

    float m20 = 2.0f * (x*z - w*y);
    float m21 = 2.0f * (y*z + w*x);
    float m22 = ww - xx - yy + zz;

    engine::Mat4 rot(1.0f);
    // glm is column-major: set columns
    rot[0] = engine::Vec4(m00, m10, m20, 0.0f);
    rot[1] = engine::Vec4(m01, m11, m21, 0.0f);
    rot[2] = engine::Vec4(m02, m12, m22, 0.0f);
    rot[3] = engine::Vec4(0.0f, 0.0f, 0.0f, 1.0f);

    this->transform_matrix = h_translate_matrix(this->_local_position.x, this->_local_position.y, this->_local_position.z)
                * rot
                * h_scale_matrix(this->_scale.x, this->_scale.y, this->_scale.z);

    this->dirty = false;
}

void Transform::copy_values_from(const Transform& other) {
    this->_local_position = other._local_position;
    this->_quaternion = other._quaternion;
    this->_scale = other._scale;
    this->transform_matrix = other.transform_matrix;
    this->dirty = other.dirty;
}

std::optional<Transform*> Transform::get_parent() {
    auto p = this->vobject_ptr->get_parent();
    if (p) {
        return &p.value()->transform();
    } else {
        return std::nullopt;
    }
}
