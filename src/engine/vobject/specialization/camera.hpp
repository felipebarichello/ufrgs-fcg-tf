#pragma once

#include "../vobject.hpp"
#include "../transform.hpp"

namespace engine {
    class Camera : public VObject, public ITransform {
        private:
            Transform transform;
        
        public:
            Camera(Transform transform) : transform(transform) {}
            Transform get_transform() override;
    };
} // namespace engine
