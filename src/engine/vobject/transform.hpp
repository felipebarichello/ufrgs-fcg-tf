#include <glm/mat4x4.hpp>

namespace engine {
    class Transform {
        private:
            glm::mat4 matrix;

        public:
            void set_position(float x, float y, float z);
            void set_rotation(float angle_in_degrees, float x, float y, float z);
            void set_scale(float x, float y, float z);

            glm::mat4 get_matrix();
    };

    class ITransform {
        public:
            virtual Transform get_transform() = 0;
    };
} // namespace engine
