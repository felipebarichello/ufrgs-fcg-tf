#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include "engine/math/matrices.hpp"
#include "engine/math/Quaternion.hpp"
#include <memory>

namespace engine {
    using engine::math::Quaternion;

    class VObject; // Forward declaration

    class Transform {
        public:
            friend class Scene;

            Transform() = default;

            Vec3 get_position() const {
                return this->_position;
            }

            Vec3& position() {
                this->dirty = true;
                return this->_position;
            }

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

            VObject* get_vobject() {
                return this->vobject_ptr;
            }

            std::optional<Transform*> get_parent();

        private:
            Vec3 _position {0.0f, 0.0f, 0.0f};
            Quaternion _quaternion;
            Vec3 _scale {1.0f, 1.0f, 1.0f};
            Mat4 transform_matrix {1.0f};
            bool dirty = false;
            VObject* vobject_ptr = nullptr;

            void update_matrix();
    };
}; // namespace engine
