//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 2
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
#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"

#include "engine/controller.hpp"
#include "engine/utils/linalg.hpp"
#include "engine/input_controller.hpp"
#include "engine/vobject/specialization/camera.hpp"
#include "engine/vao.hpp"

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
GLuint BuildCube(); // Constrói triângulos para renderização
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
//GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void update_free_camera_position();

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Funções abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informações do programa. Definidas após main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
//void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


using engine::EngineController;
using engine::WindowConfig;
using engine::Camera;
using engine::Vec3;
using engine::Transform;
using vao::Vao; 

Vao BuildCubeAxes();
Vao BuildCubeEdges();
Vao BuildCubeFaces();

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

// Abaixo definimos variáveis globais utilizadas em várias funções do código.
EngineController g_engine_controller;

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTriangles() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<const char*, SceneObject> g_VirtualScene;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
//float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;

bool g_camera_is_free = true;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = -3.14159265f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 2.5f; // Distância da câmera para a origem

const Vec3 g_camera_start_position = Vec3(0.0f, 0.0f, 2.5f);

Camera g_camera = Camera(Transform::Identity());

glm::vec4 g_free_camera_position     = glm::vec4(0.0f, 0.0f,  2.5f, 1.0f);
glm::vec4 g_free_camera_view_vector  = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
glm::vec4 g_free_camera_right_vector = glm::vec4(1.0f, 0.0f,  0.0f, 0.0f);
glm::vec2 g_free_camera_move_vector  = glm::vec2(0.0f, 0.0f);

float g_free_camera_speed = 0.1f;

bool g_input_move_forward  = false;
bool g_input_move_backward = false;
bool g_input_move_left     = false;
bool g_input_move_right    = false;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

GLuint g_vertex_array_object_id;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_render_as_black_uniform;

glm::mat4 g_the_projection;
glm::mat4 g_the_model;
glm::mat4 g_the_view;

GLFWwindow* g_window;

void update();

Vao* cube_faces_vao = nullptr;
Vao* cube_edges_vao = nullptr;
Vao* cube_axes_vao = nullptr;

int main() {
    WindowConfig window_config = WindowConfig(
        800,
        800,
        "INF01047 - 579876 - Felipe Wendt Barichello"
    );

    g_engine_controller = EngineController::start_engine(window_config);
    
    InputController& input_controller = g_engine_controller.input();
    input_controller.attach_key_handler(KeyCallback);
    input_controller.attach_mouse_button_handler(MouseButtonCallback);
    input_controller.attach_cursor_position_handler(CursorPosCallback);
    input_controller.attach_scrolling_handler(ScrollCallback);

    g_window = g_engine_controller.get_window();

    g_vertex_array_object_id = BuildCube();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    g_model_uniform            = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_render_as_black_uniform  = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "render_as_black"); // Variável booleana em shader_vertex.glsl

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    g_camera.transform().set_position(g_camera_start_position);

    g_engine_controller.hand_over_control(update);

    // Fim do programa
    return 0;
}

void update() {
    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(g_engine_controller.get_gpu_program_id());

    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTriangles(). Veja
    // comentários detalhados dentro da definição de BuildTriangles().
    glBindVertexArray(g_vertex_array_object_id);

    // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
    // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.

    glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)
    glm::vec4 camera_position_c;  // Ponto "c", centro da câmera
    glm::vec4 camera_view_vector; // Vetor "view", sentido para onde a câmera está virada

    if (g_camera_is_free) {
        // Update da posição da câmera de acordo com o input de movimento
        update_free_camera_position();

        camera_position_c = g_free_camera_position;
        camera_view_vector = g_free_camera_view_vector;

        float y = sin(g_CameraPhi);
        float z = cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = cos(g_CameraPhi)*sin(g_CameraTheta);
        
        camera_view_vector = glm::vec4(x,y,z,0.0f);
    } else {
        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        camera_position_c  = glm::vec4(x,y,z,1.0f);
        glm::vec4 camera_lookat_l = glm::vec4(0.0f,0.0f,0.0f,1.0f);    // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        camera_view_vector = camera_lookat_l - camera_position_c; 
    }

    // Computamos a matriz "View" utilizando os parâmetros da câmera para
    // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);
    g_free_camera_right_vector = glm::vec4(view[0][0], view[1][0], view[2][0], 0.0f);

    // Agora computamos a matriz de Projeção.
    glm::mat4 projection;

    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"

    if (g_UsePerspectiveProjection) {
        // Projeção Perspectiva.
        // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
        float field_of_view = 3.141592f / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_engine_controller.get_screen_ratio(), nearplane, farplane);
    } else {
        // Projeção Ortográfica.
        // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
        // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
        // Para simular um "zoom" ortográfico, computamos o valor de "t"
        // utilizando a variável g_CameraDistance.
        float t = 1.5f*g_CameraDistance/2.5f;
        float b = -t;
        float r = t*g_engine_controller.get_screen_ratio();
        float l = -r;
        projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
    }

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // Vamos desenhar 3 instâncias (cópias) do cubo
    for (int i = 1; i <= 3; ++i) {
        // Cada cópia do cubo possui uma matriz de modelagem independente,
        // já que cada cópia estará em uma posição (rotação, escala, ...)
        // diferente em relação ao espaço global (World Coordinates). Veja
        // slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 model;

        if (i == 1) {
            // A primeira cópia do cubo não sofrerá nenhuma transformação
            // de modelagem. Portanto, sua matriz "model" é a identidade, e
            // suas coordenadas no espaço global (World Coordinates) serão
            // *exatamente iguais* a suas coordenadas no espaço do modelo
            // (Model Coordinates).
            model = Matrix_Identity();
        } else if ( i == 2 ) {
            // A segunda cópia do cubo sofrerá um escalamento não-uniforme,
            // seguido de uma rotação no eixo (1,1,1), e uma translação em Z (nessa ordem!).
            model = Matrix_Translate(0.0f, 0.0f, -2.0f) // TERCEIRO translação
                    * Matrix_Rotate(3.141592f / 8.0f, glm::vec4(1.0f,1.0f,1.0f,0.0f)) // SEGUNDO rotação
                    * Matrix_Scale(2.0f, 0.5f, 0.5f); // PRIMEIRO escala
        } else if ( i == 3 ) {
            // A terceira cópia do cubo sofrerá rotações em X,Y e Z (nessa
            // ordem) seguindo o sistema de ângulos de Euler, e após uma
            // translação em X. Veja slides 106-107 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
            model = Matrix_Translate(-2.0f, 0.0f, 0.0f) // QUARTO translação
                    * Matrix_Rotate_Z(g_AngleZ)  // TERCEIRO rotação Z de Euler
                    * Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rotação Y de Euler
                    * Matrix_Rotate_X(g_AngleX); // PRIMEIRO rotação X de Euler

            // Armazenamos as matrizes model, view, e projection do terceiro cubo
            // para mostrar elas na tela através da função TextRendering_ShowModelViewProjection().
            g_the_model = model;
            g_the_projection = projection;
            g_the_view = view;
        }

        // Enviamos a matriz "model" para a placa de vídeo (GPU). Veja o
        // arquivo "shader_vertex.glsl", onde esta é efetivamente
        // aplicada em todos os pontos.
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        // Informamos para a placa de vídeo (GPU) que a variável booleana
        // "render_as_black" deve ser colocada como "false". Veja o arquivo
        // "shader_vertex.glsl".
        glUniform1i(g_render_as_black_uniform, false);

        // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
        // VAO como triângulos, formando as faces do cubo. Esta
        // renderização irá executar o Vertex Shader definido no arquivo
        // "shader_vertex.glsl", e o mesmo irá utilizar as matrizes
        // "model", "view" e "projection" definidas acima e já enviadas
        // para a placa de vídeo (GPU).
        //
        // Veja a definição de g_VirtualScene["cube_faces"] dentro da
        // função BuildTriangles(), e veja a documentação da função
        // glDrawElements() em http://docs.gl/gl3/glDrawElements.
        glDrawElements(
            g_VirtualScene["cube_faces"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
            g_VirtualScene["cube_faces"].num_indices,
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cube_faces"].first_index
        );

        // Pedimos para OpenGL desenhar linhas com largura de 4 pixels.
        glLineWidth(4.0f);

        // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
        // apontados pelo VAO como linhas. Veja a definição de
        // g_VirtualScene["axes"] dentro da função BuildTriangles(), e veja
        // a documentação da função glDrawElements() em
        // http://docs.gl/gl3/glDrawElements.
        //
        // Importante: estes eixos serão desenhamos com a matriz "model"
        // definida acima, e portanto sofrerão as mesmas transformações
        // geométricas que o cubo. Isto é, estes eixos estarão
        // representando o sistema de coordenadas do modelo (e não o global)!
        glDrawElements(
            g_VirtualScene["axes"].rendering_mode,
            g_VirtualScene["axes"].num_indices,
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["axes"].first_index
        );

        // Informamos para a placa de vídeo (GPU) que a variável booleana
        // "render_as_black" deve ser colocada como "true". Veja o arquivo
        // "shader_vertex.glsl".
        glUniform1i(g_render_as_black_uniform, true);

        // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
        // VAO como linhas, formando as arestas pretas do cubo. Veja a
        // definição de g_VirtualScene["cube_edges"] dentro da função
        // BuildTriangles(), e veja a documentação da função
        // glDrawElements() em http://docs.gl/gl3/glDrawElements.
        glDrawElements(
            g_VirtualScene["cube_edges"].rendering_mode,
            g_VirtualScene["cube_edges"].num_indices,
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cube_edges"].first_index
        );

        // Desenhamos um ponto de tamanho 15 pixels em cima do terceiro vértice
        // do terceiro cubo. Este vértice tem coordenada de modelo igual à
        // (0.5, 0.5, 0.5, 1.0).
        if ( i == 3 ) {
            glPointSize(15.0f);
            glDrawArrays(GL_POINTS, 3, 1);
        }
    }

    // Agora queremos desenhar os eixos XYZ de coordenadas GLOBAIS.
    // Para tanto, colocamos a matriz de modelagem igual à identidade.
    // Veja slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    glm::mat4 model = Matrix_Identity();

    // Enviamos a nova matriz "model" para a placa de vídeo (GPU). Veja o
    // arquivo "shader_vertex.glsl".
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));

    // Pedimos para OpenGL desenhar linhas com largura de 10 pixels.
    glLineWidth(10.0f);

    // Informamos para a placa de vídeo (GPU) que a variável booleana
    // "render_as_black" deve ser colocada como "false". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(g_render_as_black_uniform, false);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene["axes"] dentro da função BuildTriangles(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["axes"].rendering_mode,
        g_VirtualScene["axes"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["axes"].first_index
    );

    glBindVertexArray(0);

    // Pegamos um vértice com coordenadas de modelo (0.5, 0.5, 0.5, 1) e o
    // passamos por todos os sistemas de coordenadas armazenados nas
    // matrizes the_model, the_view, e the_projection; e escrevemos na tela
    // as matrizes e pontos resultantes dessas transformações.
    glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);
}

void update_free_camera_position() {
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.y * g_free_camera_view_vector;
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.x * g_free_camera_right_vector;
}

// Constrói triângulos para futura renderização
GLuint BuildCube() {
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
    // homogêneas.  Veja as seguintes referências:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //

    vao::Position vertex_positions[] = {
        // Vértices de um cubo
        //    X      Y     Z     W
        vao::Position(-0.5f,  0.5f,  0.5f, 1.0f), // posição do vértice 0
        vao::Position(-0.5f, -0.5f,  0.5f, 1.0f), // posição do vértice 1
        vao::Position( 0.5f, -0.5f,  0.5f, 1.0f), // posição do vértice 2
        vao::Position( 0.5f,  0.5f,  0.5f, 1.0f), // posição do vértice 3
        vao::Position(-0.5f,  0.5f, -0.5f, 1.0f), // posição do vértice 4
        vao::Position(-0.5f, -0.5f, -0.5f, 1.0f), // posição do vértice 5
        vao::Position( 0.5f, -0.5f, -0.5f, 1.0f), // posição do vértice 6
        vao::Position( 0.5f,  0.5f, -0.5f, 1.0f), // posição do vértice 7
        // Vértices para desenhar o eixo X
        //    X      Y     Z     W
        vao::Position( 0.0f,  0.0f,  0.0f, 1.0f), // posição do vértice 8
        vao::Position( 1.0f,  0.0f,  0.0f, 1.0f), // posição do vértice 9
        // Vértices para desenhar o eixo Y
        //    X      Y     Z     W
        vao::Position( 0.0f,  0.0f,  0.0f, 1.0f), // posição do vértice 10
        vao::Position( 0.0f,  1.0f,  0.0f, 1.0f), // posição do vértice 11
        // Vértices para desenhar o eixo Z
        //    X      Y     Z     W
        vao::Position( 0.0f,  0.0f,  0.0f, 1.0f), // posição do vértice 12
        vao::Position( 0.0f,  0.0f,  1.0f, 1.0f), // posição do vértice 13
    };

    const size_t VERTEX_POSITIONS_SIZE = sizeof(vertex_positions);
    const size_t VERTEX_POSITION_COUNT = VERTEX_POSITIONS_SIZE / sizeof(vao::Position);

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posição.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_model_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, VERTEX_POSITIONS_SIZE, NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_POSITIONS_SIZE, vertex_positions);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    vao::Color vertex_colors[] = {
    // Cores dos vértices do cubo
    //  R     G     B     A
        vao::Color(1.0f, 0.5f, 0.0f, 1.0f), // cor do vértice 0
        vao::Color(1.0f, 0.5f, 0.0f, 1.0f), // cor do vértice 1
        vao::Color(0.0f, 0.5f, 1.0f, 1.0f), // cor do vértice 2
        vao::Color(0.0f, 0.5f, 1.0f, 1.0f), // cor do vértice 3
        vao::Color(1.0f, 0.5f, 0.0f, 1.0f), // cor do vértice 4
        vao::Color(1.0f, 0.5f, 0.0f, 1.0f), // cor do vértice 5
        vao::Color(0.0f, 0.5f, 1.0f, 1.0f), // cor do vértice 6
        vao::Color(0.0f, 0.5f, 1.0f, 1.0f), // cor do vértice 7
    // Cores para desenhar o eixo X
        vao::Color(1.0f, 0.0f, 0.0f, 1.0f), // cor do vértice 8
        vao::Color(1.0f, 0.0f, 0.0f, 1.0f), // cor do vértice 9
    // Cores para desenhar o eixo
        vao::Color(0.0f, 1.0f, 0.0f, 1.0f), // cor do vértice 10
        vao::Color(0.0f, 1.0f, 0.0f, 1.0f), // cor do vértice 11
    // Cores para desenhar o eixo Z
        vao::Color(0.0f, 0.0f, 1.0f, 1.0f), // cor do vértice 12
        vao::Color(0.0f, 0.0f, 1.0f, 1.0f), // cor do vértice 13
    };

    const size_t VERTEX_COLORS_SIZE = sizeof(vertex_colors);
    const size_t VERTEX_COLOR_COUNT = VERTEX_COLORS_SIZE / sizeof(vao::Color);

    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_COLORS_SIZE, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_COLORS_SIZE, vertex_colors);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir polígonos utilizando os vértices do array
    // model_coefficients.
    //
    // Para referência sobre os modos de renderização, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint topology[] = {
    // Definimos os índices dos vértices que definem as FACES de um cubo
    // através de 12 triângulos que serão desenhados com o modo de renderização
    // GL_TRIANGLES.
        0, 1, 2, // triângulo 1 
        7, 6, 5, // triângulo 2 
        3, 2, 6, // triângulo 3 
        4, 0, 3, // triângulo 4 
        4, 5, 1, // triângulo 5 
        1, 5, 6, // triângulo 6 
        0, 2, 3, // triângulo 7 
        7, 5, 4, // triângulo 8 
        3, 6, 7, // triângulo 9 
        4, 3, 7, // triângulo 10
        4, 1, 0, // triângulo 11
        1, 6, 2, // triângulo 12
    // Definimos os índices dos vértices que definem as ARESTAS de um cubo
    // através de 12 linhas que serão desenhadas com o modo de renderização
    // GL_LINES.
        0, 1, // linha 1 
        1, 2, // linha 2 
        2, 3, // linha 3 
        3, 0, // linha 4 
        0, 4, // linha 5 
        4, 7, // linha 6 
        7, 6, // linha 7 
        6, 2, // linha 8 
        6, 5, // linha 9 
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
    // Z, que serão desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 36;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere às arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (void*)(36*sizeof(GLuint)); // Primeiro índice está em indices[36]
    cube_edges.num_indices    = 24; // Último índice está em indices[59]; total de 24 índices.
    cube_edges.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (void*)(60*sizeof(GLuint)); // Primeiro índice está em indices[60]
    axes.num_indices    = 6; // Último índice está em indices[65]; total de 6 índices.
    axes.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.
    g_VirtualScene["axes"] = axes;

    // Criamos um buffer OpenGL para armazenar os índices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(topology), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array topology[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(topology), topology);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

vao::Vao BuildCubeFaces()
{
    GLfloat face_positions[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
    };

    GLfloat face_colors[] = {
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
    };

    GLuint face_indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 7, 6,
        6, 5, 4,
        // Top face
        0, 3, 7,
        7, 4, 0,
        // Bottom face
        1, 5, 6,
        6, 2, 1,
        // Left face
        0, 4, 5,
        5, 1, 0,
        // Right face
        3, 2, 6,
        6, 7, 3
    };

    vao::VaoBuilder faces_builder;
    faces_builder.add_vbo(0, 4, sizeof(face_positions), face_positions, GL_STATIC_DRAW);
    faces_builder.add_vbo(1, 4, sizeof(face_colors), face_colors, GL_STATIC_DRAW);
    faces_builder.add_ebo(sizeof(face_indices), face_indices, GL_STATIC_DRAW);
    vao::Vao faces_vao = faces_builder.build(GL_TRIANGLES, sizeof(face_indices)/sizeof(GLuint), GL_UNSIGNED_INT);

    return faces_vao;
}

// Build the edges VAO and register it
vao::Vao BuildCubeEdges()
{
    GLfloat edge_positions[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
    };

    GLfloat edge_colors[] = {
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
    };

    GLuint edge_indices[] = {
        0,1, 1,2, 2,3, 3,0, 0,4, 4,7, 7,6, 6,2, 6,5, 5,4, 5,1, 7,3
    };

    vao::VaoBuilder edges_builder;
    edges_builder.add_vbo(0, 4, sizeof(edge_positions), edge_positions, GL_STATIC_DRAW);
    edges_builder.add_vbo(1, 4, sizeof(edge_colors), edge_colors, GL_STATIC_DRAW);
    edges_builder.add_ebo(sizeof(edge_indices), edge_indices, GL_STATIC_DRAW);
    vao::Vao edges_vao = edges_builder.build(GL_LINES, sizeof(edge_indices)/sizeof(GLuint), GL_UNSIGNED_INT);

    return edges_vao;
}

// Build the axes VAO and register it
vao::Vao BuildCubeAxes()
{
    GLfloat axes_positions[] = {
            // X axis
            0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            // Y axis
            0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            // Z axis
            0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
        };

    GLfloat axes_colors[] = {
        1.0f,0.0f,0.0f,1.0f,
        1.0f,0.0f,0.0f,1.0f,
        0.0f,1.0f,0.0f,1.0f,
        0.0f,1.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,
    };

    GLuint axes_indices[] = { 0,1, 2,3, 4,5 };

    vao::VaoBuilder axes_builder;
    axes_builder.add_vbo(0, 4, sizeof(axes_positions), axes_positions, GL_STATIC_DRAW);
    axes_builder.add_vbo(1, 4, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW);
    axes_builder.add_ebo(sizeof(axes_indices), axes_indices, GL_STATIC_DRAW);
    vao::Vao axes_vao = axes_builder.build(GL_LINES, sizeof(axes_indices)/sizeof(GLuint), GL_UNSIGNED_INT);

    return axes_vao;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
}

void update_free_camera_view_vector() {
    g_free_camera_view_vector = glm::vec4(
        cosf(g_CameraPhi) * sinf(g_CameraTheta),
        sinf(g_CameraPhi),
        cosf(g_CameraPhi) * cosf(g_CameraTheta),
        0.0f
    );
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (!g_LeftMouseButtonPressed)
        return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = (float)(xpos - g_LastCursorPosX);
    float dy = (float)(ypos - g_LastCursorPosY);

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= 0.01f*dx;

    if (g_camera_is_free) {
        g_CameraPhi   -= 0.01f*dy;
    } else {
        g_CameraPhi   += 0.01f*dy;
    }

    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;

    update_free_camera_view_vector();
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f * (float)yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

void update_free_camera_move_vector() {
    g_free_camera_move_vector = glm::vec2(0.0f, 0.0f);

    if (g_input_move_forward)
        g_free_camera_move_vector.y += 1.0f;
    if (g_input_move_backward)
        g_free_camera_move_vector.y -= 1.0f;
    if (g_input_move_left)
        g_free_camera_move_vector.x -= 1.0f;
    if (g_input_move_right)
        g_free_camera_move_vector.x += 1.0f;

    // Normalize
    if (g_free_camera_move_vector != glm::vec2(0.0f, 0.0f))
        g_free_camera_move_vector = glm::normalize(g_free_camera_move_vector);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod) {
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

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    float delta = 3.141592f / 16.0f; // 22.5 graus, em radianos.

    switch (key) {
        case GLFW_KEY_X:
            if (action == GLFW_PRESS) {
                g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_Y:
            if (action == GLFW_PRESS) {
                g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_Z:
            if (action == GLFW_PRESS) {
                g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                g_AngleX = 0.0f;
                g_AngleY = 0.0f;
                g_AngleZ = 0.0f;
            }
            break;
        case GLFW_KEY_P:
            if (action == GLFW_PRESS) {
                g_UsePerspectiveProjection = true;
            }
            break;
        case GLFW_KEY_O:
            if (action == GLFW_PRESS) {
                g_UsePerspectiveProjection = false;
            }
            break;
        case GLFW_KEY_H:
            if (action == GLFW_PRESS) {
                g_ShowInfoText = !g_ShowInfoText;
            }
            break;
        case GLFW_KEY_F:
            if (action == GLFW_PRESS)
                g_camera_is_free = !g_camera_is_free;
            break;
        case GLFW_KEY_W:
            if (action == GLFW_PRESS) {
                g_input_move_forward = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_forward = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS) {
                g_input_move_backward = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_backward = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS) {
                g_input_move_left = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_left = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS) {
                g_input_move_right = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_right = false;
                update_free_camera_move_vector();
            }
            break;
    }
}
