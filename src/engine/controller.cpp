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

        engine_controller.window = EngineController::init_window(window_config);

        engine_controller.input_controller = InputController(engine_controller.window);

        return engine_controller;
    }

    bool EngineController::update_and_test_should_close() {
        if (glfwWindowShouldClose(this->window)) {
            return true;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear framebuffer with the color above and reset the depth buffer (Z-buffer)

        return false;
    }

    GLFWwindow* EngineController::get_window() {
        return this->window;
    }

    InputController& EngineController::input() {
        return this->input_controller;
    }

    GLFWwindow* EngineController::init_window(WindowConfig window_config) {
        glfwSetErrorCallback(EngineController::error_callback);

        // Hint to use OpenGL version 3.3 or higher
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        // Hint to use a modern OpenGL profile, with only modern functionalities
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(window_config.width, window_config.height, window_config.title, NULL, NULL);
        if (!window) {
            glfwTerminate();
            fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
            std::exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window); // OpenGL calls will be rendered in this window

        // Definimos a função de callback que será chamada sempre que a janela for
        // redimensionada, por consequência alterando o tamanho do "framebuffer"
        // (região de memória onde são armazenados os pixels da imagem).
        glfwSetFramebufferSizeCallback(window, EngineController::frame_buffer_size_callback);

        // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
        // biblioteca GLAD.
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        return window;
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
