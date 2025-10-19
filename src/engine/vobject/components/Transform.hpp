#pragma once

#include "../../math/linalg.hpp"
#include "../Component.hpp"
#include <memory>

namespace engine {
    class ITransform {
        public:
            virtual Mat4 get_matrix() = 0;
    };

    class Transform : public Component {
        public:
            Mat4 get_matrix() { return transform_impl->get_matrix(); }
        
        private:
            std::unique_ptr<ITransform> transform_impl;
    };
} // namespace engine
