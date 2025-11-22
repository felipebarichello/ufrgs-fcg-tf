#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace engine {
    void init_text_renderer();
    void render_text(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
    float set_text_line_height(GLFWwindow* window);
    float set_text_char_width(GLFWwindow* window);
}
