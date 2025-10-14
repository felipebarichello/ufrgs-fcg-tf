#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace engine {

    struct DPad {
        glm::vec2* direction;
        bool forward_key_is_down = false;
        bool backward_key_is_down = false;
        bool left_key_is_down = false;
        bool right_key_is_down = false;
    };

    class InputController {
        private:
            GLFWwindow* window;
            std::map<std::pair<int,int>, std::vector<std::function<void()>>> key_handler_map;
            std::vector<DPad> dpads;
            glm::vec2 cursor_position;
        public:
            InputController(GLFWwindow* window);
            void init();
            void subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key);
            void subscribe_hold_button(int key, bool* is_down);
            void subscribe_press_button(int key, std::function<void()>);
            void attach_mouse_button_handler(GLFWmousebuttonfun handler);
            void attach_cursor_position_handler(GLFWcursorposfun handler);
            void attach_scrolling_handler(GLFWscrollfun handler);
            glm::vec2 get_cursor_position();
        private:
            void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
            void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
            void update_dpad_direction(DPad* dpad);
            void add_key_handler(int key, int action, std::function<void()>);
    };
}