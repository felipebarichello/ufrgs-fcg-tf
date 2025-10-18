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
            glm::vec2 last_cursor_position;
            glm::vec2 cursor_delta;
            float scroll_offset = 0.0f;
            bool is_left_mouse_button_down = false;
            bool focused = false;
        public:
            InputController(GLFWwindow* window);
            void init();
            void update();
            void subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key);
            void subscribe_hold_button(int key, bool* is_down);
            void subscribe_press_button(int key, std::function<void()>);
            void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
            float get_scroll_offset();
            bool left_mouse_button_is_down();
            glm::vec2 get_cursor_position();
            glm::vec2 get_cursor_position_delta();
        private:
            void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
            void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
            void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
            void update_dpad_direction(DPad* dpad);
            void add_key_handler(int key, int action, std::function<void()>);
    };
}