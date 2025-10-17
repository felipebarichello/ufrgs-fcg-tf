#pragma once

#include "../../math/linalg.hpp"

namespace engine {
    class Transform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class ITransform {
        public:
            virtual Transform& transform() = 0;
    };
} // namespace engine
