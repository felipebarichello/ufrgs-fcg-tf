#include <engine>
#include <memory>
#include "ObjLoader.hpp"
#include <cstdio>
#include <map>
#include <sstream>
#include <stdexcept>

namespace engine {

    float EngineController::delta_time = 0.0f;
    // Definition of static members declared in controller.hpp
    // Define the singleton instance (was declared in the header but not defined)
    std::unique_ptr<EngineController> EngineController::instance;
    
    // Constructor and destructor definitions - needed here where ObjLoader is complete
    EngineController::EngineController() 
        : is_fullscreen(false), windowed_width(800), windowed_height(800), windowed_xpos(0), windowed_ypos(0) {}
    
    EngineController::~EngineController() = default;
    float EngineController::screen_ratio = 1.0f;
    GLuint EngineController::gouraud_program_id = 0;
    GLuint EngineController::phong_program_id = 0;
    GLuint EngineController::star_program_id = 0;
    GLuint EngineController::particle_program_id = 0;
    std::vector<Drawable*> EngineController::drawables;

    // Helper to map ShaderType to program id
    GLuint EngineController::get_program_id(EngineController::ShaderType type) {
        switch (type) {
            case ShaderType::Gouraud:  return gouraud_program_id;
            case ShaderType::Phong:    return phong_program_id;
            case ShaderType::Star:     return star_program_id;
            case ShaderType::Particle: return particle_program_id;
            default: return 0u;
        }
    }

    EngineController* EngineController::start_engine(WindowConfig window_config) {
        EngineController::instance = std::make_unique<EngineController>();
        EngineController::instance->window = EngineController::init_window(window_config);

        // TODO: Is this needed?
        EngineController::instance->windowed_width = window_config.width;
        EngineController::instance->windowed_height = window_config.height;

        EngineController::instance->input_controller = std::make_unique<InputController>(EngineController::instance->window);
        EngineController::instance->input_controller->init();
        return EngineController::instance.get();
    }

    EngineController* EngineController::get_instance() {
        return EngineController::instance.get();
    }

    void EngineController::hand_over_control(SceneBoot* initial_scene) {
        // TODO: Scenes get created twice because this is not a constructor
        this->current_scene = std::make_unique<Scene>();

        SceneRoot hierarchy;
        initial_scene->hierarchy(hierarchy);
        
        for (auto& vobject_config : hierarchy.root_vobjects) {
            this->current_scene->instantiate(vobject_config);
        }

        while (!this->update_and_test_should_close()) {
            // Update delta time
            float current_time = static_cast<float>(glfwGetTime()); 
            EngineController::delta_time = current_time - EngineController::last_frame_time;
            EngineController::last_frame_time = current_time;

            this->input_controller->update();
            this->current_scene->update();
            EngineController::draw(); // TODO: That should not be static
            glfwSwapBuffers(this->window);
        }

        glfwTerminate();
    }

    bool EngineController::update_and_test_should_close() {
        glfwPollEvents();

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

    InputController* EngineController::input() {
        return this->input_controller.get();
    }

    InputController* EngineController::get_input() {
        return EngineController::instance->input_controller.get();
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

        // Enable anti-aliasing with 4x multisampling
        glfwWindowHint(GLFW_SAMPLES, 4);

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
        
        // Enable multisampling for anti-aliasing
        glEnable(GL_MULTISAMPLE);
        
        EngineController::load_shaders_from_files();

        return window;
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    void EngineController::error_callback(int error, const char* description) {
        (void)error; // Supress unused parameter warning
        fprintf(stderr, "ERROR: GLFW: %s\n", description);
    };

    // Definição da função que será chamada sempre que a janela do sistema
    // operacional for redimensionada, por consequência alterando o tamanho do
    // "framebuffer" (região de memória onde são armazenados os pixels da imagem).
    void EngineController::frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
        (void)window; // Supress unused parameter warning

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
        std::string exe_dir = get_executable_directory();
        
        // Load Gouraud shaders
        std::string vertex_shader_path = exe_dir + "/../../src/engine/shaders/GouraudVertexShader.glsl";
        std::string fragment_shader_path = exe_dir + "/../../src/engine/shaders/GouraudFragmentShader.glsl";

        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());

        GLuint vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        GLuint fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());

        // Deletamos o programa de GPU anterior, caso ele exista.
        if (gouraud_program_id != 0)
            glDeleteProgram(gouraud_program_id);

        gouraud_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);

        // Load Phong shaders
        vertex_shader_path = exe_dir + "/../../src/engine/shaders/PhongVertexShader.glsl";
        fragment_shader_path = exe_dir + "/../../src/engine/shaders/PhongFragmentShader.glsl";

        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());

        vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());

        // Deletamos o programa de GPU anterior, caso ele exista.
        if (phong_program_id != 0)
            glDeleteProgram(phong_program_id);
        
        phong_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);

        // Load Star shaders (simple point sprite shader used only by Stars)
        vertex_shader_path = exe_dir + "/../../src/engine/shaders/StarVertexShader.glsl";
        fragment_shader_path = exe_dir + "/../../src/engine/shaders/StarFragmentShader.glsl";
    
        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());
    
        vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());
    
        if (star_program_id != 0)
            glDeleteProgram(star_program_id);
    
        star_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);
    
        // Load Particle shaders
        vertex_shader_path = exe_dir + "/../../src/engine/shaders/ParticleVertexShader.glsl";
        fragment_shader_path = exe_dir + "/../../src/engine/shaders/ParticleFragmentShader.glsl";
    
        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());
    
        vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());
    
        if (particle_program_id != 0)
            glDeleteProgram(particle_program_id);
    
        particle_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);

        // Load Star shaders (simple point sprite shader used only by Stars)
        vertex_shader_path = exe_dir + "/../../src/engine/shaders/StarVertexShader.glsl";
        fragment_shader_path = exe_dir + "/../../src/engine/shaders/StarFragmentShader.glsl";

        printf("Loading shaders from:\n");
        printf("  Vertex: %s\n", vertex_shader_path.c_str());
        printf("  Fragment: %s\n", fragment_shader_path.c_str());

        vertex_shader_id = load_shader_vertex(vertex_shader_path.c_str());
        fragment_shader_id = load_shader_fragment(fragment_shader_path.c_str());

        if (star_program_id != 0)
            glDeleteProgram(star_program_id);

        star_program_id = create_gpu_program(vertex_shader_id, fragment_shader_id);
    }

    void EngineController::add_drawable(Drawable* drawable) {
        EngineController::drawables.push_back(drawable);
    }

    void EngineController::remove_drawable(Drawable* drawable) {
        EngineController::drawables.erase(
            std::remove(
                EngineController::drawables.begin(),
                EngineController::drawables.end(),
                drawable
            ),
            EngineController::drawables.end());
    }

    void EngineController::draw() {
        EngineController::update_main_camera();

        for (const auto& drawable : EngineController::drawables) {
            drawable->draw();
        }
    }

    void EngineController::update_main_camera() {
        auto main_camera = Camera::get_main();
        Mat4 projection = main_camera->get_perspective_matrix();
        Mat4 view = main_camera->get_view_matrix();

        glUseProgram(EngineController::get_instance()->get_gouraud_program_id());
        glUniformMatrix4fv(EngineController::instance->g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(EngineController::instance->g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        glUseProgram(EngineController::get_instance()->get_phong_program_id());
        glUniformMatrix4fv(EngineController::instance->g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(EngineController::instance->g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        if (star_program_id != 0) {
            glUseProgram(star_program_id);
            GLint view_loc = glGetUniformLocation(star_program_id, "view");
            GLint proj_loc = glGetUniformLocation(star_program_id, "projection");
            if (view_loc != -1) glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
            if (proj_loc != -1) glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
        }

        if (particle_program_id != 0) {
            glUseProgram(particle_program_id);
            GLint view_loc = glGetUniformLocation(particle_program_id, "view");
            GLint proj_loc = glGetUniformLocation(particle_program_id, "projection");
            if (view_loc != -1) glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
            if (proj_loc != -1) glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
        }

        glBindVertexArray(0);
    }

    void EngineController::toggle_fullscreen() {
        if (this->is_fullscreen) {
            // Switch to windowed mode
            glfwSetWindowMonitor(this->window, nullptr, 
                                this->windowed_xpos, this->windowed_ypos,
                                this->windowed_width, this->windowed_height, 
                                GLFW_DONT_CARE);
            this->is_fullscreen = false;
        } else {
            // Store current window position and size
            glfwGetWindowPos(this->window, &this->windowed_xpos, &this->windowed_ypos);
            glfwGetWindowSize(this->window, &this->windowed_width, &this->windowed_height);
            
            // Get the primary monitor and its video mode
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            
            // Switch to fullscreen mode
            glfwSetWindowMonitor(this->window, monitor, 
                                0, 0, mode->width, mode->height, 
                                mode->refreshRate);
            this->is_fullscreen = true;
        }
    }

    float EngineController::get_delta_time() {
        return EngineController::delta_time;
    }
} // namespace engine
