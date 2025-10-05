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
        static float screen_ratio;
        static void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
        static void error_callback(int error, const char* description);
    public:
    EngineController() : window(nullptr), input_controller(nullptr) {};

        static EngineController start_engine(WindowConfig window_config);

        // TODO: Rename to `update_and_test_should_close`
        /// @brief Ticks the engine
        /// @return True if window should close
        bool tick();

        GLFWwindow* get_window();
        InputController& input();
        float get_screen_ratio() { return screen_ratio; }
        void set_screen_ratio(float ratio) { screen_ratio = ratio; }
    };
}
