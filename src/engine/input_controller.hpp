#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <iostream>
#include <vector>
#include <functional>
#include <utility>

struct KeyAction {
    int key;
    int action;
};

class InputController {
private:
    GLFWwindow* window;
    static std::map<std::pair<int,int>, std::vector<std::function<void()>>> key_action_handler_map;
public:
    InputController() = default;
    InputController static start(GLFWwindow *window);
    void attach_mouse_button_handler(GLFWmousebuttonfun handler);
    void attach_cursor_position_handler(GLFWcursorposfun handler);
    void attach_scrolling_handler(GLFWscrollfun handler);
    void subscribe(int key, int action, std::function<void()>);
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
    size_t hash(int key, int action);
};
