#include "Quaternion.hpp"

#include <sstream>
#include <cmath>

namespace engine::math {

    Quaternion::Quaternion() {
        Quaternion_setIdentity(&this->inner);
    }

    Quaternion::Quaternion(double w_, double v1, double v2, double v3) {
        Quaternion_set(w_, v1, v2, v3, &this->inner);
    }

    Quaternion::Quaternion(const Quaternion& other) {
        Quaternion_copy(const_cast<CQuaternion*>(&other.inner), &this->inner);
    }

    Quaternion& Quaternion::operator=(const Quaternion& other) {
        if (this != &other) {
            Quaternion_copy(const_cast<CQuaternion*>(&other.inner), &this->inner);
        }
        return *this;
    }

    Quaternion Quaternion::identity() {
        return Quaternion();
    }

    Quaternion Quaternion::from_axis_angle(const Vec3& axis, double angle) {
        Quaternion out;
        double a[3] = { axis.x, axis.y, axis.z };
        Quaternion_fromAxisAngle(a, angle, &out.inner);
        return out;
    }

    double Quaternion::from_axis_angle(Vec3& outAxis) const {
        double a[3];
        double angle = Quaternion_toAxisAngle(const_cast<CQuaternion*>(&this->inner), a);
        outAxis = Vec3((float)a[0], (float)a[1], (float)a[2]);
        return angle;
    }

    Quaternion Quaternion::from_euler_zyx(const Vec3& eulerZYX) {
        Quaternion out;
        double e[3] = { eulerZYX.x, eulerZYX.y, eulerZYX.z };
        Quaternion_fromEulerZYX(e, &out.inner);
        return out;
    }

    Vec3 Quaternion::to_euler_zyx() const {
        double e[3];
        Quaternion_toEulerZYX(const_cast<CQuaternion*>(&this->inner), e);
        return Vec3((float)e[0], (float)e[1], (float)e[2]);
    }

    Quaternion Quaternion::from_x_rotation(double angle) {
        Quaternion out;
        Quaternion_fromXRotation(angle, &out.inner);
        return out;
    }

    Quaternion Quaternion::from_y_rotation(double angle) {
        Quaternion out;
        Quaternion_fromYRotation(angle, &out.inner);
        return out;
    }

    Quaternion Quaternion::from_z_rotation(double angle) {
        Quaternion out;
        Quaternion_fromZRotation(angle, &out.inner);
        return out;
    }

    // Robust conversion from two vectors (not necessarily perfectly unit) to
    // a quaternion that rotates `source` to `target`.
    Quaternion Quaternion::from_unit_vectors(const Vec3& source, const Vec3& target) {
        // Normalize inputs to guard against drift
        Vec3 u = glm::normalize(source);
        Vec3 v = glm::normalize(target);

        // Clamp dot to [-1,1] to avoid NaNs from numeric drift
        double dot = glm::clamp(glm::dot(u, v), -1.0f, 1.0f);

        // Vectors are (almost) the same
        if (dot > 0.999999f) {
            return Quaternion::identity();
        }

        // Vectors are opposite — return a 180deg rotation around any orthogonal axis
        if (dot < -0.999999f) {
            Vec3 orth = glm::cross(u, Vec3(1.0f, 0.0f, 0.0f));
            if (glm::length(orth) < 1e-6f) {
                orth = glm::cross(u, Vec3(0.0f, 1.0f, 0.0f));
            }
            orth = glm::normalize(orth);
            return Quaternion::from_axis_angle(orth, M_PI);
        }

        // General case
        Vec3 cross = glm::cross(u, v);
        double w = std::sqrt((1.0 + dot) * 0.5);
        double inv_2w = 1.0 / (2.0 * w);

        Quaternion q(w, cross.x * inv_2w, cross.y * inv_2w, cross.z * inv_2w);
        // Ensure numerical normalization
        return q.normalized();
    }

    double Quaternion::norm() const {
        return Quaternion_norm(const_cast<CQuaternion*>(&this->inner));
    }

    Quaternion Quaternion::normalized() const {
        Quaternion out;
        Quaternion_normalize(const_cast<CQuaternion*>(&this->inner), &out.inner);
        return out;
    }

    void Quaternion::normalize() {
        CQuaternion tmp;
        Quaternion_normalize(&this->inner, &tmp);
        this->inner = tmp;
    }

    Quaternion Quaternion::conjugated() const {
        Quaternion out;
        Quaternion_conjugate(const_cast<CQuaternion*>(&this->inner), &out.inner);
        return out;
    }

    Quaternion Quaternion::operator*(const Quaternion& rhs) const {
        Quaternion out;
        Quaternion_multiply(const_cast<CQuaternion*>(&this->inner), const_cast<CQuaternion*>(&rhs.inner), &out.inner);
        return out;
    }

    void Quaternion::multiply(const Quaternion& rhs, Quaternion& out) const {
        Quaternion_multiply(const_cast<CQuaternion*>(&this->inner), const_cast<CQuaternion*>(&rhs.inner), &out.inner);
    }

    void Quaternion::global_compose(const Quaternion& multiplier) const {
        Quaternion& self = const_cast<Quaternion&>(*this);
        self = multiplier * self;
    }

    void Quaternion::local_compose(const Quaternion& multiplier) const {
        Quaternion& self = const_cast<Quaternion&>(*this);
        self = self * multiplier;
    }

    Vec3 Quaternion::rotate(const Vec3& v) const {
        double iv[3] = { v.x, v.y, v.z };
        double outv[3];
        Quaternion_rotate(const_cast<CQuaternion*>(&this->inner), iv, outv);
        return Vec3((float)outv[0], (float)outv[1], (float)outv[2]);
    }

    Quaternion Quaternion::slerp(const Quaternion& q1, const Quaternion& q2, double t) {
        Quaternion out;
        Quaternion_slerp(const_cast<CQuaternion*>(&q1.inner), const_cast<CQuaternion*>(&q2.inner), t, &out.inner);
        return out;
    }

    bool Quaternion::operator==(const Quaternion& other) const {
        return Quaternion_equal(const_cast<CQuaternion*>(&this->inner), const_cast<CQuaternion*>(&other.inner));
    }

    void Quaternion::fprint(FILE* file) const {
        Quaternion_fprint(file, const_cast<CQuaternion*>(&this->inner));
    }

    std::string Quaternion::to_string() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    double Quaternion::w() const { return this->inner.w; }

    Vec3 Quaternion::v() const { return Vec3((float)this->inner.v[0], (float)this->inner.v[1], (float)this->inner.v[2]); }

    std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
        os << "Quaternion(w=" << q.w() << ", v=(" << q.v().x << ", " << q.v().y << ", " << q.v().z << "))";
        return os;
    }

} // namespace engine::math


