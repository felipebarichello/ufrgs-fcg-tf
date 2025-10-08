#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>


struct DPadState {
    glm::vec2* direction;
    bool forward_key_is_down = false;
    bool backward_key_is_down = false;
    bool left_key_is_down = false;
    bool right_key_is_down = false;
};

class InputController {
private:
    GLFWwindow* window;
    static std::map<std::pair<int,int>, std::vector<std::function<void()>>> key_action_handler_map;
    static std::vector<DPadState> dpad_states;
    static void update_dpad_state(DPadState& dpad_state);

public:
    InputController() = default;
    static InputController init(GLFWwindow *window);
    void attach_mouse_button_handler(GLFWmousebuttonfun handler);
    void attach_cursor_position_handler(GLFWcursorposfun handler);
    void attach_scrolling_handler(GLFWscrollfun handler);
    void subscribe_key_action(int key, int action, std::function<void()>);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
    void subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key);
};
