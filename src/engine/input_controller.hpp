#pragma once

#include <GLFW/glfw3.h>

class InputController {
private:
    GLFWwindow* window;

public:
    InputController(GLFWwindow* window) : window(window) {}
    void attach_key_handler(GLFWkeyfun handler);
    void attach_mouse_button_handler(GLFWmousebuttonfun handler);
    void attach_cursor_position_handler(GLFWcursorposfun handler);
    void attach_scrolling_handler(GLFWscrollfun handler);
};
