#pragma once

#include "linalg.hpp" // for engine::Vec3
#include <ostream>
#include <array>

// Include the C quaternion header at global scope with C linkage so the
// implementation in lib-src compiles to the expected C symbols. We remap
// the typedef name `Quaternion` to `CQuaternion` while including so the
// plain identifier `Quaternion` is not introduced into the global scope
// as a typedef (that name would collide with our C++ wrapper). The C
// struct tag remains `struct Quaternion` which does not affect linking.
#define Quaternion CQuaternion
extern "C" {
#include "Quaternion/Quaternion.h"
}
#undef Quaternion

namespace engine::math {
    using CQuaternion = ::CQuaternion;

class Quaternion {
        public:
            // Default constructs identity quaternion
            Quaternion();

            // Construct from components
            Quaternion(double w, double v1, double v2, double v3);

            // Copy / move
            Quaternion(const Quaternion& other);
            Quaternion(Quaternion&&) noexcept = default;
            Quaternion& operator=(const Quaternion& other);
            Quaternion& operator=(Quaternion&&) noexcept = default;

            // Factory
            static Quaternion identity();
            static Quaternion fromAxisAngle(const Vec3& axis, double angle);
            double toAxisAngle(Vec3& outAxis) const; // returns angle, fills outAxis

            static Quaternion fromEulerZYX(const Vec3& eulerZYX);
            Vec3 toEulerZYX() const;

            static Quaternion fromXRotation(double angle);
            static Quaternion fromYRotation(double angle);
            static Quaternion fromZRotation(double angle);

            static Quaternion fromUnitVectors(const Vec3& source, const Vec3& target);

            double norm() const;
            Quaternion normalized() const;
            void normalize_inplace();

            Quaternion conjugated() const;

            // Multiplication (rotation composition)
            Quaternion operator*(const Quaternion& rhs) const;
            Quaternion& operator*=(const Quaternion& rhs) {
                this->multiply(rhs, *this);
                return *this;
            }
            void multiply(const Quaternion& rhs, Quaternion& out) const; // out = (*this) * rhs

            // Rotate a vector by this quaternion
            Vec3 rotate(const Vec3& v) const;

            // Spherical linear interpolation
            static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t);

            // Equality (uses QUATERNION_EPS)
            bool operator==(const Quaternion& other) const;
            bool operator!=(const Quaternion& other) const { return !(*this == other); }

            // Print helpers
            void fprint(FILE* file) const;
            std::string to_string() const;

            // Access raw components
            double w() const;
            Vec3 v() const;

            // Access underlying C struct (for advanced usage)
            const CQuaternion* c_ptr() const { return &inner; }
            CQuaternion* c_ptr() { return &inner; }

        private:
            CQuaternion inner;
    };

    // ostream support
    std::ostream& operator<<(std::ostream& os, const Quaternion& q);

} // namespace engine::math
