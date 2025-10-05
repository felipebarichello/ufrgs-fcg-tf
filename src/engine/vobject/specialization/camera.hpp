#pragma once

#include "../vobject.hpp"
#include "../transform.hpp"

namespace engine {
    class Camera : public VObject, public ITransform {
        private:
            Transform _transform;
        
        public:
            Camera();
            Transform& transform() override;
            void set_transform(Transform transform);

            /// @brief Sets the camera vectors given an up vector and a view vector.
            /// Assumes the vectors are orthonormal.
            void set_vectors_from_up_view(Vec3 up, Vec3 view);
    };
} // namespace engine
