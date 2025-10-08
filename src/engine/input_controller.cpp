#include "input_controller.hpp"

std::map<std::pair<int,int>, std::vector<std::function<void()>>> InputController::key_action_handler_map;
std::vector<DPad> InputController::dpads;

InputController InputController::init(GLFWwindow *window) {
    InputController input_controller = InputController();
    input_controller.window = window;
    glfwSetKeyCallback(input_controller.window, InputController::key_callback);
    return input_controller;
}

void InputController::subscribe_key_action(int key, int action, std::function<void()> function) {
    key_action_handler_map[{key,action}].push_back(function);
}

void InputController::subscribe_press_button(int key, std::function<void()> function) {
    subscribe_key_action(key, GLFW_PRESS, function);
}

void InputController::subscribe_hold_button(int key, bool* is_down) {
    subscribe_key_action(key, GLFW_PRESS, [is_down]() { *is_down = true; });
    subscribe_key_action(key, GLFW_RELEASE, [is_down]() { *is_down = false; });
}

void InputController::subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key) {
    // Store the state for this vector
    dpads.push_back(DPad());
    DPad& dpad = dpads.back();

    dpad.direction = direction;
    dpad.forward_key = forward_key;
    dpad.backward_key = backward_key;
    dpad.left_key = left_key;
    dpad.right_key = right_key;

    // capture index by value to avoid dangling reference
    subscribe_hold_button(forward_key, &dpad.forward_key_is_down);
    subscribe_hold_button(backward_key, &dpad.backward_key_is_down);
    subscribe_hold_button(left_key, &dpad.left_key_is_down);
    subscribe_hold_button(right_key, &dpad.right_key_is_down);
}

void InputController::key_callback(GLFWwindow *window, int key, int scancode, int action, int mod)
{
    // =======================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // =======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    for (std::function<void()> function : key_action_handler_map[{key,action}]) {
        function();
    }

    for (DPad& dpad : dpads) {
        update_dpad_direction(dpad);
    }

}

void InputController::update_dpad_direction(DPad& dpad) {

    *(dpad.direction) = glm::vec2(0.0f, 0.0f);

    if (dpad.forward_key_is_down)
        dpad.direction->y += 1.0f;
    if (dpad.backward_key_is_down)
        dpad.direction->y -= 1.0f;
    if (dpad.left_key_is_down)
        dpad.direction->x -= 1.0f;
    if (dpad.right_key_is_down)
        dpad.direction->x += 1.0f;

    // Normalize
    if (*(dpad.direction) != glm::vec2(0.0f, 0.0f))
        *(dpad.direction) = glm::normalize(*(dpad.direction));
}

void InputController::attach_mouse_button_handler(GLFWmousebuttonfun handler)
{
    glfwSetMouseButtonCallback(this->window, handler);
}

void InputController::attach_cursor_position_handler(GLFWcursorposfun handler)
{
    glfwSetCursorPosCallback(this->window, handler);
}

void InputController::attach_scrolling_handler(GLFWscrollfun handler)
{
    glfwSetScrollCallback(this->window, handler);
}