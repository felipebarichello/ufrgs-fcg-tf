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

            Mat4 get_view_matrix() {
                return invert_orthonormal_matrix(this->get_vobject()->transform().get_model_matrix());
            }

            Mat4 get_perspective_matrix() const;

            float near_distance = -0.1f;
            float far_distance  = -1000000.0f;
            float fov = M_PI / 3.0f;

        private:
            inline static Camera* main = nullptr;
    };
} // namespace engine
