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
#include <engine>

#include "game/scenes/MainScene.hpp"

#include <ctime>

#include <temp_globals.hpp>

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
//void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

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
using engine::Matrix_Translate;
using engine::Matrix_Rotate;
using engine::Matrix_Rotate_X;
using engine::Matrix_Rotate_Y;
using engine::Matrix_Rotate_Z;
using engine::Matrix_Scale;
using engine::InputController;
using engine::ObjLoader;
using game::scenes::MainScene;

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject {
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

// Abaixo definimos variáveis globais utilizadas em várias funções do código.
EngineController* g_engine_controller;

Vec3 g_free_camera_right_vector     = Vec3(1.0f, 0.0f, 0.0f);
Vec3 g_free_camera_up_vector        = Vec3(0.0f, 1.0f, 0.0f);
Vec2 g_free_camera_move_vector      = Vec2(0.0f, 0.0f);

void update();

int main() {
    // TODO: Move configuration to game?
    g_engine_controller = EngineController::start_engine(WindowConfig(
        800,
        800,
        "FCG - Trabalho Final"
    ));
    
    // Subscribe F11 key to toggle fullscreen
    g_engine_controller->input()->subscribe_press_button(GLFW_KEY_F11, [&]() {
        g_engine_controller->toggle_fullscreen();
    });

    g_engine_controller->g_view_uniform       = glGetUniformLocation(g_engine_controller->get_gpu_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_engine_controller->g_projection_uniform = glGetUniformLocation(g_engine_controller->get_gpu_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    EventManager& events = g_engine_controller->events();
    events.subscribe_update(update);

    std::unique_ptr<MainScene> initial_scene = std::make_unique<MainScene>();
    g_engine_controller->hand_over_control(initial_scene.get());
    return 0;
}

void update() {
    auto main_camera = Camera::get_main();
    // main_camera->set_basis_from_up_view(g_free_camera_up_vector, camera_view_unit_vector);
    Mat4 view = invert_orthonormal_matrix(main_camera->get_vobject()->transform().get_model_matrix());
    main_camera->view = view;
    g_free_camera_right_vector = Vec3(view[0][0], view[1][0], view[2][0]);
}
