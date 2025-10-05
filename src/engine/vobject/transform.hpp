#include "../utils/linalg.hpp"

namespace engine {
    class Transform {
        private:
            Mat4 matrix;

        public:
            static Transform Identity();

            Transform(Mat4 matrix) : matrix(matrix) {}

            void set_position(Vec3 position);
            void set_rotation(float angle_in_radians, Vec3 axis);
            void set_scale(Vec3 scale);

            Mat4 get_matrix();
            void set_matrix(Mat4 matrix);

            /// @brief Sets the basis vectors of the transform matrix.
            /// The vectors should be orthonormal.
            void set_basis_vectors(Vec3 u, Vec3 v, Vec3 w);
    };

    class ITransform {
        public:
            virtual Transform& transform() = 0;
    };
} // namespace engine
