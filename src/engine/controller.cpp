#include "controller.hpp"
#include <cstdio>
#include <map>
#include <sstream>


namespace engine {

    // Definition of static member declared in controller.hpp
    float EngineController::screen_ratio = 1.0f;

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
        glfwSetErrorCallback(EngineController::error_callback);

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

        engine_controller.input_controller = InputController(engine_controller.window);

        // Definimos a função de callback que será chamada sempre que a janela for
        // redimensionada, por consequência alterando o tamanho do "framebuffer"
        // (região de memória onde são armazenados os pixels da imagem).
        glfwSetFramebufferSizeCallback(engine_controller.window, EngineController::frame_buffer_size_callback);

        // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
        glfwMakeContextCurrent(engine_controller.window);

        // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
        // biblioteca GLAD.
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        return engine_controller;
    }

    bool EngineController::tick() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        return glfwWindowShouldClose(this->window);
    }

    GLFWwindow* EngineController::get_window() {
        return this->window;
    }

    InputController& EngineController::input() {
        return this->input_controller;
    }


    // Definimos o callback para impressão de erros da GLFW no terminal
    void EngineController::error_callback(int error, const char* description) {
        fprintf(stderr, "ERROR: GLFW: %s\n", description);
    };

    // Definição da função que será chamada sempre que a janela do sistema
    // operacional for redimensionada, por consequência alterando o tamanho do
    // "framebuffer" (região de memória onde são armazenados os pixels da imagem).
    void EngineController::frame_buffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // Indicamos que queremos renderizar em toda região do framebuffer. A
        // função "glViewport" define o mapeamento das "normalized device
        // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
        // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
        glViewport(0, 0, width, height);

        // Atualizamos também a razão que define a proporção da janela (largura /
        // altura), a qual será utilizada na definição das matrizes de projeção,
        // tal que não ocorra distorções durante o processo de "Screen Mapping"
        // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
        //
        // O cast para float é necessário pois números inteiros são arredondados ao
        // serem divididos!
        EngineController::screen_ratio = (float) width / height;
    }

} // namespace engine
