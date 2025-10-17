#pragma once

#include "../vobject.hpp"
#include "transform.hpp"

namespace engine {
    struct CameraTransform : public Transform {
        Vec3 position;
        HyperVec3 basis;

        static CameraTransform Identity();

        CameraTransform(Vec3 position, HyperVec3 basis)
            : position(position), basis(basis) {}

        void set_position(Vec3 position);
        Mat4 get_matrix() override;

        /// @brief Sets the camera vectors given an up vector and a view vector.
        /// Assumes the vectors are orthonormal.
        void set_basis_from_up_view(Vec3 up, Vec3 view);
    };

    class Camera : public ITransform {
        private:
            CameraTransform _transform;
        
        public:
            Camera();
            Transform& transform() override;
            CameraTransform& cam_transform();
            void set_transform(Transform transform);
    };
} // namespace engine
