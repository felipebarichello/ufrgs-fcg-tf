#pragma once

#include "../../math/linalg.hpp"
#include "../Component.hpp"

namespace engine {
    class Transform : public Component {
        public:
            virtual Mat4 get_matrix() = 0;
    };
} // namespace engine
