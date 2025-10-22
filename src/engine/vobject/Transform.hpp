#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include "engine/math/matrices.hpp"
#include "engine/math/Quaternion.hpp"
#include <memory>

namespace engine {
    using engine::math::Quaternion;

    class ITransform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class Transform {
        public:
            Transform();
            void set_position(Vec3 position);
            void set_rotation(const Quaternion& q);
            void set_scale(Vec3 scale);
            void set_scale(float uniform_scale);
            Mat4 get_model_matrix();

        private:
            Vec3 position {0.0f, 0.0f, 0.0f};
            Quaternion quaternion = Quaternion::identity();
            Vec3 scale {1.0f, 1.0f, 1.0f};
            Mat4 model_matrix {1.0f};
            bool dirty_model_matrix = false;

            void update_model_matrix();
    };
}; // namespace engine
