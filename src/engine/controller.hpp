#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

namespace engine {
    
    class EngineController {
    private:
        GLFWwindow* window;

    public:
        EngineController() = default;

        void set_window(GLFWwindow* win) {
            this->window = win;
        }
        
        static EngineController start_engine();

        /// @brief Ticks the engine
        /// @return True if window should close
        bool tick();
    };
}
