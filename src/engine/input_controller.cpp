#include "input_controller.hpp"

void InputController::attach_key_handler(GLFWkeyfun handler) {
    glfwSetKeyCallback(this->window, handler);
}

void InputController::attach_mouse_button_handler(GLFWmousebuttonfun handler) {
    glfwSetMouseButtonCallback(this->window, handler);
}

void InputController::attach_cursor_position_handler(GLFWcursorposfun handler) {
    glfwSetCursorPosCallback(this->window, handler);
}

void InputController::attach_scrolling_handler(GLFWscrollfun handler) {
    glfwSetScrollCallback(this->window, handler);
}
