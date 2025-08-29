//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 1
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <string>
#include <fstream>
#include <sstream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"


#define PI 3.14159265358979323846

namespace felib {
    typedef struct Position {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;

        Position() {
            x = 0.0;
            y = 0.0;
            z = 0.0;
            w = 0.0;
        }

        Position(GLfloat x, GLfloat y, GLfloat z) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = 1.0;
        }

        Position(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
    } Position;

    typedef struct Color {
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;

        Color() {
            r = 0.0;
            g = 0.0;
            b = 0.0;
            a = 0.0;
        }

        Color(GLfloat r, GLfloat g, GLfloat b) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = 1.0;
        }

        Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    } Color;

    typedef struct Vertex {
        Position position;
        Color color;

        Vertex(Position position, Color color) {
            this->position = position;
            this->color = color;
        }
    } Vertex;

    template<size_t N>
    class GeometryEditor {
    private:
        Position (&vertex_positions)[N];
        Color (&vertex_colors)[N];

    public:
        GeometryEditor(Position (&vertex_positions)[N], Color (&vertex_colors)[N])
            : vertex_positions(vertex_positions), vertex_colors(vertex_colors) {}

        void set(size_t index, Vertex vertex) {
            this->vertex_positions[index] = vertex.position;
            this->vertex_colors[index] = vertex.color;
        }
    };

    typedef struct Vao {
        GLuint vao_id;
        GLenum topology_mode;
        GLsizei topology_size;
        GLenum topology_type;

        Vao(GLuint vao_id, GLenum topology_mode, GLsizei topology_size, GLenum topology_type) {
            this->vao_id = vao_id;
            this->topology_mode = topology_mode;
            this->topology_size = topology_size;
            this->topology_type = topology_type;
        }

        void bind() const {
            glBindVertexArray(this->vao_id);
        }

        void unbind() const {
            glBindVertexArray(0);
        }
    } Vao;

    typedef class VaoBuilder {
    private:
        GLuint vao_id;

    public:
        VaoBuilder() {
            glGenVertexArrays(1, &this->vao_id);
            glBindVertexArray(this->vao_id);
        }

        ~VaoBuilder() {
            glBindVertexArray(0);
        }

        VaoBuilder& add_vbo(GLuint location, GLint number_of_dimensions, size_t buffer_size, void* data, GLenum usage_hint) {
            this->add_buffer(GL_ARRAY_BUFFER, buffer_size, data, usage_hint);
            glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(location);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return *this;
        }

        VaoBuilder& add_ebo(size_t buffer_size, void* data, GLenum usage_hint) {
            this->add_buffer(GL_ELEMENT_ARRAY_BUFFER, buffer_size, data, usage_hint);
            return *this;
        }

        Vao build(GLenum topology_mode, GLsizei topology_size, GLenum topology_type) {
            return Vao(this->vao_id, topology_mode, topology_size, topology_type);
        }

    private:
        void add_buffer(GLenum target, size_t buffer_size, void* data, GLenum usage_hint) {
            GLuint vbo_id;
            glGenBuffers(1, &vbo_id);
            glBindBuffer(target, vbo_id);
            glBufferData(target, buffer_size, data, usage_hint);
        }
    } VaoBuilder;
}


// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
felib::Vao build_vao(); // Constrói triângulos para renderização
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;

int main()
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 500 colunas e 500 linhas
    // de pixels, e com título "INF01047 ...".
    glfwWindowHint(GLFW_SAMPLES, 16); // Request 16x MSAA
    GLFWwindow* window = glfwCreateWindow(500, 500, "INF01047 - 00579876 - Felipe Wendt Barichello", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado.
    glfwSetKeyCallback(window, KeyCallback);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    // Construímos a representação de um triângulo
    felib::Vao vao = build_vao();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima
        glClear(GL_COLOR_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // vértices apontados pelo VAO criado pela função build_vao(). Veja
        // comentários detalhados dentro da definição de build_vao().
        vao.bind();

        // Pedimos para a GPU rasterizar os vértices apontados pelo VAO como
        // triângulos.
        glDrawElements(vao.topology_mode, vao.topology_size, vao.topology_type, 0);

        // "Desligamos" o VAO, evitando assim que operações posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        vao.unbind();

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Constrói triângulos para futura renderização
felib::Vao build_vao()
{
    using felib::VaoBuilder;
    using felib::Position;
    using felib::Color;
    using felib::GeometryEditor;


    /* High level geometry parameters */

    const size_t CIRCUMFERENCE_VERTEX_COUNT = 800;
    const GLfloat VERTICAL_OUTER_RADIUS = 0.4f;
    const GLfloat INNER_RADIUS_RATIO = 0.7f;
    const GLfloat HORIZONTAL_RADIUS_RATIO = 0.5f;

    const Color OUTER_VERTEX_COLOR = Color(0.0, 0.0, 1.0);
    const Color INNER_VERTEX_COLOR = Color(0.0, 0.0, 1.0);


    /* Derived geometry constants */

    const size_t VERTEX_COUNT = 2 * CIRCUMFERENCE_VERTEX_COUNT;

    Position vertex_positions[VERTEX_COUNT];
    const size_t VERTEX_POSITIONS_SIZE = sizeof(vertex_positions);

    Color vertex_colors[VERTEX_COUNT];
    const size_t VERTEX_COLORS_SIZE = sizeof(vertex_colors);
    
    
    /* Geometry creation */

    GeometryEditor<VERTEX_COUNT> geometry = GeometryEditor<VERTEX_COUNT>(vertex_positions, vertex_colors);

    // Make vertices
    for (size_t i = 0; i < CIRCUMFERENCE_VERTEX_COUNT; i++) {
        const GLfloat CENTER_X = 0.0f;
        const GLfloat CENTER_Y = 0.0f;

        const GLfloat VERTICAL_INNER_RADIUS = INNER_RADIUS_RATIO * VERTICAL_OUTER_RADIUS;
        const GLfloat HORIZONTAL_OUTER_RADIUS = HORIZONTAL_RADIUS_RATIO * VERTICAL_OUTER_RADIUS;
        const GLfloat HORIZONTAL_INNER_RADIUS = HORIZONTAL_RADIUS_RATIO * VERTICAL_INNER_RADIUS;

        size_t outer_vertex_index = 2*i;
        size_t inner_vertex_index = outer_vertex_index + 1;

        GLfloat angle  = (GLfloat)(2.0f * PI * (GLfloat)i / (GLfloat)CIRCUMFERENCE_VERTEX_COUNT);
        GLfloat cosine = (GLfloat)cos(angle);
        GLfloat sine   = (GLfloat)sin(angle);

        geometry.set(outer_vertex_index, felib::Vertex(
            Position(
                HORIZONTAL_OUTER_RADIUS * cosine,
                VERTICAL_OUTER_RADIUS * sine,
                0.0
            ),
            OUTER_VERTEX_COLOR
        ));

        geometry.set(inner_vertex_index, felib::Vertex(
            Position(
                HORIZONTAL_INNER_RADIUS * cosine,
                VERTICAL_INNER_RADIUS * sine,
                0.0
            ),
            INNER_VERTEX_COLOR
        ));
    }


    /* Topology */

    const GLenum TOPOLOGY_MODE = GL_TRIANGLE_STRIP;

    const size_t TOPOLOGY_LENGTH = VERTEX_COUNT + 2;
    GLushort topology[TOPOLOGY_LENGTH];
    const GLsizei TOPOLOGY_SIZE = sizeof(topology);

    for (size_t i = 0; i < VERTEX_COUNT; i++) {
        topology[i] = (GLushort)i;
    }

    topology[TOPOLOGY_LENGTH - 2] = 0;
    topology[TOPOLOGY_LENGTH - 1] = 1;


    return VaoBuilder()
        .add_vbo(0, 4, VERTEX_POSITIONS_SIZE, vertex_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, VERTEX_COLORS_SIZE,    vertex_colors,    GL_STATIC_DRAW)
        .add_ebo(      TOPOLOGY_SIZE,         topology,         GL_STATIC_DRAW)
        .build(TOPOLOGY_MODE, TOPOLOGY_LENGTH, GL_UNSIGNED_SHORT);
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
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

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
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
        output += "== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retorna o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slides 141-148 do documento Aula_03_Rendering_Pipeline_Grafico.pdf).
    glViewport(0, 0, width, height);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // =======================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // =======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// vim: set spell spelllang=pt_br :

