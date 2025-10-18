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
        glfwSetScrollCallback(this->window, [](GLFWwindow* w, double xoffset, double yoffset) {
            InputController* controller = static_cast<InputController*>(glfwGetWindowUserPointer(w));
            if (controller) {
                controller->scroll_callback(w, xoffset, yoffset);
            }
        });
        this->subscribe_hold_button(GLFW_MOUSE_BUTTON_LEFT, &this->is_left_mouse_button_down);
    }
}

void InputController::add_key_handler(int key, int action, std::function<void()> function) {
    key_handler_map[{key,action}].push_back(function);
}

void InputController::subscribe_press_button(int key, std::function<void()> function) {
    add_key_handler(key, GLFW_PRESS, function);
}

void InputController::subscribe_hold_button(int key, bool* is_down) {
    add_key_handler(key, GLFW_PRESS, [is_down]() { *is_down = true; });
    add_key_handler(key, GLFW_RELEASE, [is_down]() { *is_down = false; });
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

float InputController::get_scroll_offset() {
    return this->scroll_offset;
}

bool InputController::left_mouse_button_is_down() {
    return this->is_left_mouse_button_down;
}

void InputController::key_callback(GLFWwindow *window, int key, int scancode, int action, int mod) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
        this->focused = false;
    }

    if (this->focused) {
        for (std::function<void()> function : key_handler_map[{key,action}]) {
            function();
        }
    }
}

void InputController::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        this->focused = true;
    }

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
    if (!focused) {
        return glm::vec2(0.0f, 0.0f);
    } else {
        return delta;
    }
}

void InputController::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    this->cursor_position.x = (float)xpos;
    this->cursor_position.y = (float)ypos;
}

void InputController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    this->scroll_offset = (float)yoffset;
}