#include "controller.hpp"
#include <cstdio>
#include <map>
#include <sstream>

// Definimos o callback para impressão de erros da GLFW no terminal
void error_callback(int error, const char* description) {
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

namespace engine {
    EngineController EngineController::start_engine(WindowConfig window_config) {
        EngineController engine_controller = EngineController();

        // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
        // sistema operacional, onde poderemos renderizar com OpenGL.
        int success = glfwInit();
        if (!success) {
            fprintf(stderr, "ERROR: glfwInit() failed.\n");
            std::exit(EXIT_FAILURE);
        }

        // Definimos o callback para impressão de erros da GLFW no terminal
        glfwSetErrorCallback(error_callback);

        // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
        // funções modernas de OpenGL.
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        engine_controller.window = glfwCreateWindow(window_config.width, window_config.height, window_config.title, NULL, NULL);
        if (!engine_controller.window) {
            glfwTerminate();
            fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
            std::exit(EXIT_FAILURE);
        }

        return engine_controller;
    }

    bool EngineController::tick() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        return glfwWindowShouldClose(this->window);
    }

    GLFWwindow* EngineController::get_window() {
        return this->window;
    }
} // namespace engine
