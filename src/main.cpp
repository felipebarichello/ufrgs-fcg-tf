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

//GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void update_free_camera_position();

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

const float g_sensitivity = 0.005f;
const float field_of_view = 3.141592f / 3.0f;

void start();
void update();

int main() {
    // TODO: Move configuration to game?
    g_engine_controller = EngineController::start_engine(WindowConfig(
        800,
        800,
        "FCG - Trabalho Final"
    ));

    g_engine_controller.input()->subscribe_dpad(&g_free_camera_move_vector, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
    g_engine_controller.input()->subscribe_dpad(&g_free_camera_move_vector, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT);

    Cube cube1 = Cube();
    Cube cube2 = Cube();
    Cube cube3 = Cube();
    Cube cube4 = Cube();
    Sphere sphere1 = Sphere();

    cube2.set_position(Vec3(0.0f, 0.0f, -2.0f));
    cube2.set_scale(Vec3(2.0f, 0.5f, 0.5f));
    cube2.set_rotation(3.141592f / 8.0f, Vec3(1.0f,1.0f,1.0f));

    cube3.set_position(Vec3(-2.0f, 0.0f, 0.0f));

    cube4.set_position(Vec3(0.0f, 0.0f, 7.0f));
    cube4.set_scale(Vec3(3.0f, 3.0f, 3.0f));

    g_model_uniform      = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    //g_engine_controller.add_drawable(&cube1);
    // g_engine_controller.add_drawable(&cube2);
    // g_engine_controller.add_drawable(&cube3);
    // g_engine_controller.add_drawable(&cube4);
    g_engine_controller.add_drawable(&sphere1);

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

    CameraTransform cam_transform;
    cam_transform.set_position(camera_position_c);
    cam_transform.set_basis_from_up_view(g_free_camera_up_vector, camera_view_unit_vector);
    glm::mat4 view = invert_orthonormal_matrix(cam_transform.get_matrix());
    g_free_camera_right_vector = Vec3(view[0][0], view[1][0], view[2][0]);

    glm::mat4 projection;

    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -400.0f; // Posição do "far plane"

    projection = Matrix_Perspective(field_of_view, g_engine_controller.get_screen_ratio(), nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    glBindVertexArray(0);
}

void update_free_camera_position() {
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.y * g_free_camera_view_unit_vector;
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.x * g_free_camera_right_vector;
}

void update_free_camera_direction() {

    // if (!g_engine_controller.input()->left_mouse_button_is_down())
    //     return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    glm::vec2 cursor_delta = g_engine_controller.input()->get_cursor_position_delta();

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= g_sensitivity*cursor_delta.x;

    if (g_camera_is_free) {
        g_CameraPhi   -= g_sensitivity*cursor_delta.y;
    } else {
        g_CameraPhi   += g_sensitivity*cursor_delta.y;
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
