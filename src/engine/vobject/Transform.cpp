#include "Transform.hpp"
#include "VObject.hpp"

using namespace engine;

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

    glm::mat4 rot(1.0f);
    // glm is column-major: set columns
    rot[0] = glm::vec4(m00, m10, m20, 0.0f);
    rot[1] = glm::vec4(m01, m11, m21, 0.0f);
    rot[2] = glm::vec4(m02, m12, m22, 0.0f);
    rot[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    this->transform_matrix = Matrix_Translate(this->_position.x, this->_position.y, this->_position.z)
                * rot
                * Matrix_Scale(this->_scale.x, this->_scale.y, this->_scale.z);

    this->dirty = false;
}

std::optional<Transform*> Transform::get_parent() {
    auto p = this->vobject_ptr->get_parent();
    if (p) {
        return &p.value()->transform();
    } else {
        return std::nullopt;
    }
}
