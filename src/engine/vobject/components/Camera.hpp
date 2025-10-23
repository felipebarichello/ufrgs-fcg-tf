#pragma once

#include <engine/vobject/Component.hpp>
#include <engine/vobject/VObject.hpp>
#include <engine/vobject/Transform.hpp>
#include <cmath>

namespace engine {
    class Camera : public Component {
        public:
            static Camera* get_main() {
                return Camera::main;
            }

            static void set_main(Camera* camera) {
                Camera::main = camera;
            }

            Mat4 get_perspective_matrix() const;

            float near_distance = -0.1f;
            float far_distance  = -400.0f;
            float fov = M_PI / 3.0f;

            Mat4 view; // TODO: Take that out of my sight

        private:
            inline static Camera* main = nullptr;
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
