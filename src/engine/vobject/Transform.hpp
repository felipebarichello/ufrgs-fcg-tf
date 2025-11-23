#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include "engine/math/linalg.hpp"
#include "engine/math/Quaternion.hpp"
#include <memory>

namespace engine {
    using engine::Quaternion;

    class VObject; // Forward declaration

    class Transform {
        public:
            friend class Scene;

            Transform() = default;

            Vec3 get_local_position() const {
                return this->_local_position;
            }

            Vec3& local_position() {
                this->dirty = true;
                return this->_local_position;
            }

            Vec3 get_world_position() {
                Mat4 model = this->get_model_matrix();
                return Vec3(model[3].x, model[3].y, model[3].z);
            }

            void set_world_position(const Vec3& pos);

            Quaternion get_quaternion() const {
                return this->_quaternion;
            }

            Quaternion& quaternion() {
                this->dirty = true;
                return this->_quaternion;
            }

            Vec3 get_scale() const {
                return this->_scale;
            }
            
            Vec3& scale() {
                this->dirty = true;
                return this->_scale;
            }

            Mat4 get_model_matrix();

            /// @brief Copy everything but the `VObject*`
            void copy_values_from(const Transform&);

            VObject* get_vobject() {
                return this->vobject_ptr;
            }

            std::optional<Transform*> get_parent();

        private:
            Vec3 _local_position {0.0f, 0.0f, 0.0f};
            Quaternion _quaternion;
            Vec3 _scale {1.0f, 1.0f, 1.0f};
            Mat4 transform_matrix {1.0f};
            bool dirty = false;
            VObject* vobject_ptr = nullptr;

            void update_matrix();
    };
}; // namespace engine
