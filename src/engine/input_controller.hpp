#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>


struct VectorState {
    glm::vec2* key_vector;
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
};

class InputController {
private:
    GLFWwindow* window;
    static std::map<std::pair<int,int>, std::vector<std::function<void()>>> key_action_handler_map;
    bool input_move_forward = false;
    bool input_move_backward = false;
    bool input_move_left = false;
    bool input_move_right = false;
    static std::vector<VectorState> vector_state;

    static void OnKeyForward_Press(VectorState& vector_state);
    static void OnKeyForward_Release(VectorState& vector_state);
    static void OnKeyBackward_Press(VectorState& vector_state);
    static void OnKeyBackward_Release(VectorState& vector_state);
    static void OnKeyLeft_Press(VectorState& vector_state);
    static void OnKeyLeft_Release(VectorState& vector_state);
    static void OnKeyRight_Press(VectorState& vector_state);
    static void OnKeyRight_Release(VectorState& vector_state);
    static void update_key_move_vector(VectorState& vector_state);

public:
    InputController() = default;
    static InputController start(GLFWwindow *window);
    void attach_mouse_button_handler(GLFWmousebuttonfun handler);
    void attach_cursor_position_handler(GLFWcursorposfun handler);
    void attach_scrolling_handler(GLFWscrollfun handler);
    void subscribe_key_action(int key, int action, std::function<void()>);
    static void  key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
    void subscribe_key_move_vector(glm::vec2* key_vector, int forward_key, int backward_key, int left_key, int right_key);

};
