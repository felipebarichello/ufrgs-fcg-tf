#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "input_controller.hpp"

namespace engine {
    struct WindowConfig {
        int width;
        int height;
        const char* title;

        WindowConfig(int width, int height, const char* title)
            : width(width), height(height), title(title) {}
    };
    
    class EngineController {
    private:
        GLFWwindow* window;
        InputController input_controller;

    public:
        EngineController() : window(nullptr), input_controller(nullptr) {};

        static EngineController start_engine(WindowConfig window_config);

        /// @brief Ticks the engine
        /// @return True if window should close
        bool tick();

        GLFWwindow* get_window();
        InputController& input();
    };
}
