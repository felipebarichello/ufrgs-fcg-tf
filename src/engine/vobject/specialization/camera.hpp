#pragma once

#include "../vobject.hpp"
#include "../transform.hpp"

namespace engine {
    class Camera : public VObject, public ITransform {
        private:
            Transform _transform;
        
        public:
            Camera(Transform transform) : _transform(transform) {}
            Transform& transform() override;
            void set_transform(Transform transform);
    };
} // namespace engine
