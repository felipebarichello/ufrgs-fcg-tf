#include "input_controller.hpp"

namespace engine {
    class Camera {
        private:
            bool mouse_button_pressed = false;
            bool camera_is_free = true;
            float phi;
            float theta;
            const float max_phi = 3.141592f/2;
            const float min_phi = -3.141592f/2;
        public:
            Camera(InputController InputController);
        private:
            void update();
            void update_free_camera_position();
            void update_free_camera_direction();
    }
};