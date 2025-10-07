#include "input_controller.hpp"

InputController InputController::start(GLFWwindow *window) {
    InputController input_controller = InputController();
    input_controller.window = window;
    glfwSetKeyCallback(input_controller.window, InputController::key_callback);
    return input_controller;
}
void InputController::attach_key_handler(GLFWkeyfun handler)
{
    // glfwSetKeyCallback(this->window, handler);
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

    const char *action_str = nullptr;
    switch (action)
    {
    case GLFW_PRESS:
        action_str = "pressed";
        break;
    case GLFW_RELEASE:
        action_str = "released";
        break;
    case GLFW_REPEAT:
        action_str = "repeated";
        break;
    default:
        action_str = "unknown";
        break;
    }

    // Print key and action
    printf("Key %d was %s\n", key, action_str);
}
