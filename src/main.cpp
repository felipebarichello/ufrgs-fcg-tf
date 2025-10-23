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

using engine::EngineController;
using engine::WindowConfig;
using engine::InputController;
using game::scenes::MainScene;

// Abaixo definimos variáveis globais utilizadas em várias funções do código.
EngineController* g_engine_controller;

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

    std::unique_ptr<MainScene> initial_scene = std::make_unique<MainScene>();
    g_engine_controller->hand_over_control(initial_scene.get());
    
    return 0;
}
