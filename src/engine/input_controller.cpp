#include "input_controller.hpp"

std::map<std::pair<int,int>, std::vector<std::function<void()>>> InputController::key_action_handler_map;
std::vector<DPadState> InputController::dpad_states;

InputController InputController::init(GLFWwindow *window) {
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

void InputController::subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key) {
    // Store the state for this vector
    dpad_states.push_back(DPadState());
    DPadState& state = dpad_states.back();
    int index = (int)dpad_states.size() - 1;
    state.direction = direction;
    state.forward_key_is_down = false;
    state.backward_key_is_down = false;
    state.left_key_is_down = false;
    state.right_key_is_down = false;

    // capture index by value to avoid dangling reference
    subscribe_key_action(forward_key, GLFW_PRESS, [index]() { 
        dpad_states[index].forward_key_is_down = true;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(forward_key, GLFW_RELEASE, [index]() { 
        dpad_states[index].forward_key_is_down = false;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(backward_key, GLFW_PRESS, [index]() { 
        dpad_states[index].backward_key_is_down = true;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(backward_key, GLFW_RELEASE, [index]() { 
        dpad_states[index].backward_key_is_down = false;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(left_key, GLFW_PRESS, [index]() { 
        dpad_states[index].left_key_is_down = true;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(left_key, GLFW_RELEASE, [index]() { 
        dpad_states[index].left_key_is_down = false;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(right_key, GLFW_PRESS, [index]() { 
        dpad_states[index].right_key_is_down = true;
        InputController::update_dpad_state(dpad_states[index]);
    });
    subscribe_key_action(right_key, GLFW_RELEASE, [index]() { 
        dpad_states[index].right_key_is_down = false;
        InputController::update_dpad_state(dpad_states[index]);
    });
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

void InputController::update_dpad_state(DPadState& vector_state) {

    *(vector_state.direction) = glm::vec2(0.0f, 0.0f);

    if (vector_state.forward_key_is_down)
        vector_state.direction->y += 1.0f;
    if (vector_state.backward_key_is_down)
        vector_state.direction->y -= 1.0f;
    if (vector_state.left_key_is_down)
        vector_state.direction->x -= 1.0f;
    if (vector_state.right_key_is_down)
        vector_state.direction->x += 1.0f;

    // Normalize
    if (*(vector_state.direction) != glm::vec2(0.0f, 0.0f))
        *(vector_state.direction) = glm::normalize(*(vector_state.direction));
}




