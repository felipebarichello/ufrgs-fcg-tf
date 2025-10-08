#include "input_controller.hpp"

std::map<std::pair<int,int>, std::vector<std::function<void()>>> InputController::key_action_handler_map;
std::vector<VectorState> InputController::vector_state;

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

void InputController::subscribe_key_move_vector(glm::vec2* key_vector_ref, int forward_key, int backward_key, int left_key, int right_key) {
    // Store the state for this vector
    vector_state.push_back(VectorState());
    VectorState& state = vector_state.back();
    int index = (int)vector_state.size() - 1;
    state.key_vector = key_vector_ref;
    state.forward = false;
    state.backward = false;
    state.left = false;
    state.right = false;

    // capture index by value to avoid dangling reference
    subscribe_key_action(forward_key, GLFW_PRESS, [index]() { 
        vector_state[index].forward = true;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(forward_key, GLFW_RELEASE, [index]() { 
        vector_state[index].forward = false;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(backward_key, GLFW_PRESS, [index]() { 
        vector_state[index].backward = true;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(backward_key, GLFW_RELEASE, [index]() { 
        vector_state[index].backward = false;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(left_key, GLFW_PRESS, [index]() { 
        vector_state[index].left = true;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(left_key, GLFW_RELEASE, [index]() { 
        vector_state[index].left = false;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(right_key, GLFW_PRESS, [index]() { 
        vector_state[index].right = true;
        InputController::update_key_move_vector(vector_state[index]);
    });
    subscribe_key_action(right_key, GLFW_RELEASE, [index]() { 
        vector_state[index].right = false;
        InputController::update_key_move_vector(vector_state[index]);
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

void InputController::update_key_move_vector(VectorState& vector_state) {

    *(vector_state.key_vector) = glm::vec2(0.0f, 0.0f);

    if (vector_state.forward)
        vector_state.key_vector->y += 1.0f;
    if (vector_state.backward)
        vector_state.key_vector->y -= 1.0f;
    if (vector_state.left)
        vector_state.key_vector->x -= 1.0f;
    if (vector_state.right)
        vector_state.key_vector->x += 1.0f;

    // Normalize
    if (*(vector_state.key_vector) != glm::vec2(0.0f, 0.0f))
        *(vector_state.key_vector) = glm::normalize(*(vector_state.key_vector));
}




