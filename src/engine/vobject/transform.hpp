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
    };

    class ITransform {
        public:
            virtual Transform& transform() = 0;
    };
} // namespace engine
