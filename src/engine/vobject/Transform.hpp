#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include "engine/math/matrices.hpp"
#include <memory>

namespace engine {
    class ITransform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class Transform : public Component {
        public:
            Transform() = default;
            void set_position(glm::vec3 position);
            void set_rotation(float rotation_angle, glm::vec3 rotation_axis);
            void set_scale(glm::vec3 scale);
            void set_scale(float uniform_scale);
        private:
            glm::vec3 position{0.0f, 0.0f, 0.0f};
            glm::vec3 rotation_axis{0.0f, 1.0f, 0.0f};
            glm::vec3 scale{1.0f, 1.0f, 1.0f};
            float rotation_angle{0.0f};
            glm::mat4 model{1.0f};
            void update_model_matrix();
    };
}; // namespace engine
