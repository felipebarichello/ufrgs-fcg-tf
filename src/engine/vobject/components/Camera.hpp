#pragma once

#include "../Component.hpp"
#include "../VObject.hpp"
#include "Transform.hpp"

namespace engine {
    class Camera : public Component {
    };

    struct CameraTransform : public ITransform {
        Vec3 position;
        HyperVec3 basis;

        static CameraTransform Identity();

        CameraTransform() = default;

        CameraTransform(Vec3 position, HyperVec3 basis)
            : position(position), basis(basis) {}

        void set_position(Vec3 position);
        Mat4 get_matrix() override;

        /// @brief Sets the camera vectors given an up vector and a view vector.
        /// Assumes the vectors are orthonormal.
        void set_basis_from_up_view(Vec3 up, Vec3 view);
    };
} // namespace engine
