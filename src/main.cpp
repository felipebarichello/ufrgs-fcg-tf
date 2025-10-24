#include <engine>

#include "game/scenes/MainScene.hpp"

using engine::EngineController;
using engine::WindowConfig;
using engine::InputController;
using game::scenes::MainScene;

int main() {
    // TODO: Move configuration to game?
    EngineController* engine_controller = EngineController::start_engine(WindowConfig(
        800,
        800,
        "FCG - Trabalho Final"
    ));
    
    // Subscribe F11 key to toggle fullscreen
    engine_controller->input()->subscribe_press_button(GLFW_KEY_F11, [&]() {
        engine_controller->toggle_fullscreen();
    });

    // TODO: Take this out of here
    engine_controller->g_view_uniform       = glGetUniformLocation(engine_controller->get_gouraud_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    engine_controller->g_projection_uniform = glGetUniformLocation(engine_controller->get_gouraud_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    // Enable z-buffer
    // TODO: Take this out of here
    glEnable(GL_DEPTH_TEST);

    std::unique_ptr<MainScene> initial_scene = std::make_unique<MainScene>();
    engine_controller->hand_over_control(initial_scene.get());
    
    return 0;
}
