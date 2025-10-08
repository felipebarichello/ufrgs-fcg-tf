#include "input_controller.hpp"

std::map<std::pair<int,int>, std::vector<std::function<void()>>> InputController::key_action_handler_map;
glm::vec2* InputController::key_vector = nullptr;

InputController InputController::start(GLFWwindow *window) {
    InputController input_controller = InputController();
    input_controller.window = window;
    glfwSetKeyCallback(input_controller.window, InputController::key_callback);
    return input_controller;
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

void InputController::subscribe_key_action(int key, int action, std::function<void()> function) {
    key_action_handler_map[{key,action}].push_back(function);
}

void InputController::subscribe_key_move_vector(glm::vec2* key_vector_ref, int forward_key, int backward_key, int left_key, int right_key) {

    key_vector = key_vector_ref;
   
    auto OnKeyForward_Press = [&]() {
        input_move_forward = true;
        update_key_move_vector();
    };
    auto OnKeyForward_Release = [&]() {
        input_move_forward = false;
        update_key_move_vector();
    };
    auto OnKeyBackward_Press = [&]() {
        input_move_backward = true;
        update_key_move_vector();
    };
    auto OnKeyBackward_Release = [&]() {
        input_move_backward = false;
        update_key_move_vector();
    };
    auto OnKeyLeft_Press = [&]() {
        input_move_left = true;
        update_key_move_vector();
    };
    auto OnKeyLeft_Release = [&]() {
        input_move_left = false;
        update_key_move_vector();
    };
    auto OnKeyRight_Press = [&]() {
        input_move_right = true;
        update_key_move_vector();
    };
    auto OnKeyRight_Release = [&]() {
        input_move_right = false;
        update_key_move_vector();
    };

    subscribe_key_action(forward_key, GLFW_PRESS, OnKeyForward_Press);
    subscribe_key_action(forward_key, GLFW_RELEASE, OnKeyForward_Release);
    subscribe_key_action(backward_key, GLFW_PRESS, OnKeyBackward_Press);
    subscribe_key_action(backward_key, GLFW_RELEASE, OnKeyBackward_Release);
    subscribe_key_action(left_key, GLFW_PRESS, OnKeyLeft_Press);
    subscribe_key_action(left_key, GLFW_RELEASE, OnKeyLeft_Release);
    subscribe_key_action(right_key, GLFW_PRESS, OnKeyRight_Press);
    subscribe_key_action(right_key, GLFW_RELEASE, OnKeyRight_Release);
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

}

void InputController::update_key_move_vector() {

    *key_vector = glm::vec2(0.0f, 0.0f);

    if (input_move_forward)
        (*key_vector).y += 1.0f;
    if (input_move_backward)
        (*key_vector).y -= 1.0f;
    if (input_move_left)
        (*key_vector).x -= 1.0f;
    if (input_move_right)
        (*key_vector).x += 1.0f;

    // Normalize
    if (*key_vector != glm::vec2(0.0f, 0.0f))
        *key_vector = glm::normalize(*key_vector);
}
