#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

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

    public:
        EngineController() = default;

        void set_window(GLFWwindow* win) {
            this->window = win;
        }

        static EngineController start_engine(WindowConfig window_config);

        /// @brief Ticks the engine
        /// @return True if window should close
        bool tick();

        GLFWwindow* get_window();
    };
}
