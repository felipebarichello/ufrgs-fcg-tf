#include "controller.hpp"
#include <cstdio>
#include <map>
#include <sstream>


namespace engine {

    // Definition of static member declared in controller.hpp
    float EngineController::screen_ratio = 1.0f;
    GLuint EngineController::gpu_program_id = 0;

    EngineController EngineController::start_engine(WindowConfig window_config) {
        EngineController engine_controller = EngineController();

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
        int success = glfwInit();
        if (!success) {
            fprintf(stderr, "ERROR: glfwInit() failed.\n");
            std::exit(EXIT_FAILURE);
        }

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
        
        EngineController::load_shaders_from_files();

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
    // Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
    GLuint EngineController::load_shader_vertex(const char* filename)
    {
        // Criamos um identificador (ID) para este shader, informando que o mesmo
        // será aplicado nos vértices.
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

        // Carregamos e compilamos o shader
        load_shader(filename, vertex_shader_id);

        // Retorna o ID gerado acima
        return vertex_shader_id;
    }

    // Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
    GLuint EngineController::load_shader_fragment(const char* filename)
    {
        // Criamos um identificador (ID) para este shader, informando que o mesmo
        // será aplicado nos fragmentos.
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Carregamos e compilamos o shader
        load_shader(filename, fragment_shader_id);

        // Retorna o ID gerado acima
        return fragment_shader_id;
    }

    // Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
    // um arquivo GLSL e faz sua compilação.
    void EngineController::load_shader(const char* filename, GLuint shader_id)
    {
        // Lemos o arquivo de texto indicado pela variável "filename"
        // e colocamos seu conteúdo em memória, apontado pela variável
        // "shader_string".
        std::ifstream file;
        try {
            file.exceptions(std::ifstream::failbit);
            file.open(filename);
        } catch ( std::exception& e ) {
            (void)e; // Remove unused variable warning.
            fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
            std::exit(EXIT_FAILURE);
        }
        std::stringstream shader;
        shader << file.rdbuf();
        std::string str = shader.str();
        const GLchar* shader_string = str.c_str();
        const GLint   shader_string_length = static_cast<GLint>( str.length() );

        // Define o código do shader GLSL, contido na string "shader_string"
        glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

        // Compila o código do shader GLSL (em tempo de execução)
        glCompileShader(shader_id);

        // Verificamos se ocorreu algum erro ou "warning" durante a compilação
        GLint compiled_ok;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

        GLint log_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];
        glGetShaderInfoLog(shader_id, log_length, &log_length, log);

        // Imprime no terminal qualquer erro ou "warning" de compilação
        if ( log_length != 0 )
        {
            std::string  output;

            if ( !compiled_ok )
            {
                output += "ERROR: OpenGL compilation of \"";
                output += filename;
                output += "\" failed.\n";
                output += "== Start of compilation log\n";
                output += log;
                output += "== End of compilation log\n";
            }
            else
            {
                output += "WARNING: OpenGL compilation of \"";
                output += filename;
                output += "\".\n";
                output += "== Start of compilation log\n";
                output += log;
                output += "== End of compilation log\n";
            }

            fprintf(stderr, "%s", output.c_str());
        }

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;
    }

    // Esta função cria um programa de GPU, o qual contém obrigatoriamente um
    // Vertex Shader e um Fragment Shader.
    GLuint EngineController::create_gpu_program(GLuint vertex_shader_id, GLuint fragment_shader_id)
    {
        // Criamos um identificador (ID) para este programa de GPU
        GLuint program_id = glCreateProgram();

        // Definição dos dois shaders GLSL que devem ser executados pelo programa
        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);

        // Linkagem dos shaders acima ao programa
        glLinkProgram(program_id);

        // Verificamos se ocorreu algum erro durante a linkagem
        GLint linked_ok = GL_FALSE;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

        // Imprime no terminal qualquer erro de linkagem
        if ( linked_ok == GL_FALSE )
        {
            GLint log_length = 0;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

            // Alocamos memória para guardar o log de compilação.
            // A chamada "new" em C++ é equivalente ao "malloc()" do C.
            GLchar* log = new GLchar[log_length];

            glGetProgramInfoLog(program_id, log_length, &log_length, log);

            std::string output;

            output += "ERROR: OpenGL linking of program failed.\n";
            output += "== Start of link log\n";
            output += log;
            output += "\n== End of link log\n";

            // A chamada "delete" em C++ é equivalente ao "free()" do C
            delete [] log;

            fprintf(stderr, "%s", output.c_str());
        }

        // Retornamos o ID gerado acima
        return program_id;
    }

    #ifdef _WIN32
        #include <windows.h>
        #include <libloaderapi.h>
    #else
        #include <unistd.h>
        #include <limits.h>
    #endif

    // Função que obtém o diretório onde está o executável
    std::string EngineController::get_executable_directory()
    {
    #ifdef _WIN32
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        std::string exe_path(path);
        size_t pos = exe_path.find_last_of("\\/");
        return exe_path.substr(0, pos);
    #else
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count != -1) {
            path[count] = '\0';
            std::string exe_path(path);
            size_t pos = exe_path.find_last_of('/');
            return exe_path.substr(0, pos);
        }
        return ".";
    #endif
    }

    // Função que carrega os shaders de vértices e de fragmentos que serão
    // utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    void EngineController::load_shaders_from_files()
    {
        // Obtemos o diretório do executável e construímos os caminhos relativos aos shaders
        std::string exe_dir = get_executable_directory();
        
        // Construímos os caminhos para os shaders relativos ao executável
        // Assumindo estrutura: bin/Debug/main.exe e src/shader_*.glsl
        std::string vertex_shader_path = exe_dir + "/../../src/engine/shaders/shader_vertex.glsl";
        std::string fragment_shader_path = exe_dir + "/../../src/engine/shaders/shader_fragment.glsl";

        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());

        GLuint vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        GLuint fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());

        // Deletamos o programa de GPU anterior, caso ele exista.
        if (gpu_program_id != 0)
            glDeleteProgram(gpu_program_id);

        // Criamos um programa de GPU utilizando os shaders carregados acima.
        gpu_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);
    }

} // namespace engine
