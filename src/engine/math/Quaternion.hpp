#pragma once

#include "linalg.hpp" // for engine::Vec3
#include <ostream>
#include <array>

// Include the C quaternion header at global scope with C linkage so the
// implementation in lib-src compiles to the expected C symbols. This must
// be outside the `engine::math` namespace so the C typedef doesn't end up
// declaring a symbol inside the namespace and clash with our C++ class.
extern "C" {
#include "Quaternion/Quaternion.h"
}

namespace engine::math {
    using CQuaternion = ::Quaternion;

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
        void toEulerZYX(Vec3& outEulerZYX) const;

        static Quaternion fromXRotation(double angle);
        static Quaternion fromYRotation(double angle);
        static Quaternion fromZRotation(double angle);

        double norm() const;
        Quaternion normalized() const;
        void normalize_inplace();

        Quaternion conjugated() const;

        // Multiplication (rotation composition)
        Quaternion operator*(const Quaternion& rhs) const;
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
