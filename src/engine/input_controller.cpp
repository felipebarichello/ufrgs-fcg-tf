#include "input_controller.hpp"

using namespace engine;

InputController::InputController(GLFWwindow *window) {
    this->window = window;
}

void InputController::init() {
    if (this->window != nullptr) {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(this->window, [](GLFWwindow* w, int key, int scancode, int action, int mod) {
            InputController* controller = static_cast<InputController*>(glfwGetWindowUserPointer(w));
            if (controller) {
                controller->key_callback(w, key, scancode, action, mod);
            }
        });
        glfwSetCursorPosCallback(this->window, [](GLFWwindow* w, double xpos, double ypos) {
            InputController* controller = static_cast<InputController*>(glfwGetWindowUserPointer(w));
            if (controller) {
                controller->cursor_position_callback(w, xpos, ypos);
            }
        });
        glfwSetMouseButtonCallback(this->window, [](GLFWwindow* w, int button, int action, int mods) {
            InputController* controller = static_cast<InputController*>(glfwGetWindowUserPointer(w));
            if (controller) {
                controller->mouse_button_callback(w, button, action, mods);
            }
        });
    }
}

void InputController::add_key_handler(int key, int action, std::function<void()> function) {
    key_handler_map[{key,action}].push_back(function);
}

void InputController::subscribe_press_button(int key, std::function<void()> function) {
    add_key_handler(key, GLFW_PRESS, function);
}

void InputController::subscribe_hold_button(int key, bool* is_down) {
    std::cout << "Subscribing hold button for key " << key << "\n";
    add_key_handler(key, GLFW_PRESS, [is_down]() { *is_down = true; std::cout << "Pressed\n"; });
    add_key_handler(key, GLFW_RELEASE, [is_down]() { *is_down = false; std::cout << "Released\n"; });
} 

void InputController::subscribe_dpad(glm::vec2* direction, int forward_key, int backward_key, int left_key, int right_key) {
    // Store the state for this vector
    dpads.push_back(DPad());
    DPad& dpad = dpads.back();
    size_t index = dpads.size() - 1;

    dpad.direction = direction;

    add_key_handler(forward_key, GLFW_PRESS, [this,index]() { 
        dpads[index].forward_key_is_down = true;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(forward_key, GLFW_RELEASE, [this,index]() {
        dpads[index].forward_key_is_down = false;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(backward_key, GLFW_PRESS, [this,index]() {
        dpads[index].backward_key_is_down = true;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(backward_key, GLFW_RELEASE, [this,index]() {
        dpads[index].backward_key_is_down = false;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(left_key, GLFW_PRESS, [this,index]() {
        dpads[index].left_key_is_down = true;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(left_key, GLFW_RELEASE, [this,index]() {
        dpads[index].left_key_is_down = false;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(right_key, GLFW_PRESS, [this,index]() {
        dpads[index].right_key_is_down = true;
        update_dpad_direction(&dpads[index]);
    });
    add_key_handler(right_key, GLFW_RELEASE, [this,index]() {
        dpads[index].right_key_is_down = false;
        update_dpad_direction(&dpads[index]);
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

    for (std::function<void()> function : key_handler_map[{key,action}]) {
        function();
    }
}

void InputController::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    this->last_cursor_position = this->cursor_position;
    for (std::function<void()> function : key_handler_map[{button,action}]) {
        function();
    }
}

void InputController::update_dpad_direction(DPad* dpad) {

    *(dpad->direction) = glm::vec2(0.0f, 0.0f);

    if (dpad->forward_key_is_down)
        dpad->direction->y += 1.0f;
    if (dpad->backward_key_is_down)
        dpad->direction->y -= 1.0f;
    if (dpad->left_key_is_down)
        dpad->direction->x -= 1.0f;
    if (dpad->right_key_is_down)
        dpad->direction->x += 1.0f;

    // Normalize
    if (*(dpad->direction) != glm::vec2(0.0f, 0.0f))
        *(dpad->direction) = glm::normalize(*(dpad->direction));
}

glm::vec2 InputController::get_cursor_position() {
    return this->cursor_position;
}

glm::vec2 InputController::get_cursor_position_delta() {
    glm::vec2 delta = this->cursor_position - this->last_cursor_position;
    this->last_cursor_position = this->cursor_position;
    return delta;
}

void InputController::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    this->cursor_position.x = (float)xpos;
    this->cursor_position.y = (float)ypos;
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