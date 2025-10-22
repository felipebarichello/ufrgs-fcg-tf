#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include "engine/math/matrices.hpp"
#include "engine/math/Quaternion.hpp"
#include <memory>

namespace engine {
    using engine::math::Quaternion;

    class VObject; // Forward declaration

    class ITransform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class Transform {
        public:
            friend class Scene;

            Transform();

            Vec3 get_position() const {
                return this->position;
            }

            void set_position(Vec3 position);

            void add_position(Vec3 position) {
                this->position += position;
                this->dirty_transform_matrix = true;
            }

            Quaternion get_rotation() const {
                return this->quaternion;
            }

            void set_rotation(const Quaternion& q);

            Vec3 get_scale() const {
                return this->scale;
            }
            
            void set_scale(Vec3 scale);
            void set_scale(float uniform_scale);

            Mat4 get_model_matrix();

            VObject* get_vobject() {
                return this->vobject_ptr;
            }

            Transform& get_parent();

        private:
            Vec3 position {0.0f, 0.0f, 0.0f};
            Quaternion quaternion = Quaternion::identity();
            Vec3 scale {1.0f, 1.0f, 1.0f};
            Mat4 transform_matrix {1.0f};
            bool dirty_transform_matrix = false;
            VObject* vobject_ptr = nullptr;

            void update_model_matrix();
    };
}; // namespace engine
