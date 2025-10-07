#pragma once

#include <GLFW/glfw3.h>
#include "unordered_map"
#include <iostream>

class InputController {
private:
    GLFWwindow* window;
public:
    InputController() = default;
    InputController static start(GLFWwindow *window);
    void attach_key_handler(GLFWkeyfun handler);
    void attach_mouse_button_handler(GLFWmousebuttonfun handler);
    void attach_cursor_position_handler(GLFWcursorposfun handler);
    void attach_scrolling_handler(GLFWscrollfun handler);
    void subscribe(int key, int action, void (*function));
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
};
