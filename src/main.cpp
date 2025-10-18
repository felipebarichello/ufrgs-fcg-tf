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
#include <utils.h>
#include <engine.hpp>

// TODO: Remove unused function declarations

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
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
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void update_free_camera_direction(); 

using engine::EngineController;
using engine::WindowConfig;
using engine::Camera;
using engine::Vec3;
using engine::Vec2;
using engine::Mat4;
using engine::Transform;
using engine::Vao; 
using engine::VaoBuilder;
using engine::CameraTransform;
using engine::invert_orthonormal_matrix;
using engine::EventManager;
using engine::Matrix_Identity;
using engine::Matrix_Perspective;
using engine::Matrix_Translate;
using engine::Matrix_Rotate;
using engine::Matrix_Rotate_X;
using engine::Matrix_Rotate_Y;
using engine::Matrix_Rotate_Z;
using engine::Matrix_Scale;
using engine::InputController;


Vao BuildCubeAxes();
Vao BuildCubeEdges();
Vao BuildCubeFaces();

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject {
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

bool g_camera_is_free = true;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = -3.14159265f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 2.5f; // Distância da câmera para a origem

const Vec3 g_camera_start_position = Vec3(0.0f, 0.0f, 2.5f);

Camera g_camera = Camera();

Vec3 g_free_camera_position         = Vec3(0.0f, 0.0f, 2.5f);
Vec3 g_free_camera_view_unit_vector = Vec3(0.0f, 0.0f, -1.0f);
Vec3 g_free_camera_right_vector     = Vec3(1.0f, 0.0f, 0.0f);
Vec3 g_free_camera_up_vector        = Vec3(0.0f, 1.0f, 0.0f);
Vec2 g_free_camera_move_vector      = Vec2(0.0f, 0.0f);

float g_free_camera_speed = 0.1f;

bool g_input_move_forward  = false;
bool g_input_move_backward = false;
bool g_input_move_left     = false;
bool g_input_move_right    = false;

GLuint g_vertex_array_object_id;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;

Vao cube_faces_vao = Vao();
Vao cube_edges_vao = Vao();
Vao cube_axes_vao = Vao();

void start();
void update();

int main() {
    WindowConfig window_config(
        800,
        800,
        "INF01047 - 579876 - Felipe Wendt Barichello"
    );

    g_engine_controller = EngineController::start_engine(window_config);

    g_engine_controller.input()->subscribe_dpad(&g_free_camera_move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
    g_engine_controller.input()->subscribe_dpad(&g_free_camera_move_vector, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT);

    cube_faces_vao = BuildCubeFaces();
    cube_edges_vao = BuildCubeEdges();
    cube_axes_vao  = BuildCubeAxes();
    Cube the_cube = Cube();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    g_model_uniform      = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    EventManager& events = g_engine_controller.events();
    events.subscribe_update(update);

    g_engine_controller.hand_over_control();
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

    Vec3 camera_position_c; // Camera center (position)
    Vec3 camera_view_unit_vector; // Direction the camera is pointing

    if (g_camera_is_free) {
        // Update da posição da câmera de acordo com o input de movimento
        update_free_camera_position();
        update_free_camera_direction();

        camera_position_c = g_free_camera_position;
        camera_view_unit_vector = g_free_camera_view_unit_vector;

        float y = sin(g_CameraPhi);
        float z = cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = cos(g_CameraPhi)*sin(g_CameraTheta);
        
        camera_view_unit_vector = Vec3(x, y, z);
    } else {
        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        camera_position_c = Vec3(x, y, z);
        Vec3 camera_lookat_point = Vec3(0.0f, 0.0f, 0.0f);
        camera_view_unit_vector = glm::normalize(camera_lookat_point - camera_position_c); 
    }

    CameraTransform cam_transform = g_camera.cam_transform();
    cam_transform.set_position(camera_position_c);
    cam_transform.set_basis_from_up_view(g_free_camera_up_vector, camera_view_unit_vector);
    glm::mat4 view = invert_orthonormal_matrix(cam_transform.get_matrix());
    g_free_camera_right_vector = Vec3(view[0][0], view[1][0], view[2][0]);

    glm::mat4 projection;

    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592f / 3.0f;
    projection = Matrix_Perspective(field_of_view, g_engine_controller.get_screen_ratio(), nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    //the_cube.draw(g_model_uniform);

    // Vamos desenhar 3 instâncias (cópias) do cubo
    for (int i = 2; i <= 3; ++i) {
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
        }

        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        cube_faces_vao.draw();

        glLineWidth(2.0f);
        cube_axes_vao.draw();
        cube_edges_vao.draw();

        // Desenhamos um ponto de tamanho 15 pixels em cima do terceiro vértice
        // do terceiro cubo. Este vértice tem coordenada de modelo igual à
        // (0.5, 0.5, 0.5, 1.0).
        if ( i == 3 ) {
            glPointSize(15.0f);
            glDrawArrays(GL_POINTS, 3, 1);
        }
    }

    
    glm::mat4 model = Matrix_Identity();
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(10.0f);
    cube_axes_vao.draw();

    glBindVertexArray(0);

    // Pegamos um vértice com coordenadas de modelo (0.5, 0.5, 0.5, 1) e o
    // passamos por todos os sistemas de coordenadas armazenados nas
    // matrizes the_model, the_view, e the_projection; e escrevemos na tela
    // as matrizes e pontos resultantes dessas transformações.
    glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);
}

void update_free_camera_position() {
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.y * g_free_camera_view_unit_vector;
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.x * g_free_camera_right_vector;
}

Vao BuildCubeFaces()
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

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(face_positions), face_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(face_colors), face_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(face_indices), face_indices, GL_STATIC_DRAW)
        .build(GL_TRIANGLES, sizeof(face_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

// Build the edges VAO and register it
Vao BuildCubeEdges()
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

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(edge_positions), edge_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(edge_colors), edge_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(edge_indices), edge_indices, GL_STATIC_DRAW)
        .build(GL_LINES, sizeof(edge_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

// Build the axes VAO and register it
Vao BuildCubeAxes()
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

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(axes_positions), axes_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(axes_indices), axes_indices, GL_STATIC_DRAW)
        .build(GL_LINES, sizeof(axes_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

void update_free_camera_direction() {

    // if (!g_engine_controller.input()->left_mouse_button_is_down())
    //     return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    glm::vec2 cursor_delta = g_engine_controller.input()->get_cursor_position_delta();

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= 0.01f*cursor_delta.x;

    if (g_camera_is_free) {
        g_CameraPhi   -= 0.01f*cursor_delta.y;
    } else {
        g_CameraPhi   += 0.01f*cursor_delta.y;
    }

    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    g_free_camera_view_unit_vector = Vec3(
        cosf(g_CameraPhi) * sinf(g_CameraTheta),
        sinf(g_CameraPhi),
        cosf(g_CameraPhi) * cosf(g_CameraTheta)
    );
}
