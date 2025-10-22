#pragma once

#include "engine/math/linalg.hpp"
#include "engine/vobject/Component.hpp"
#include <memory>

namespace engine {
    class ITransform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class Transform : public Component {
        public:
            Transform() = default;
    };
} // namespace engine
