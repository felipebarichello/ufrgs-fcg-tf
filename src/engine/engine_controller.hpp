#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fstream>
#include <memory>
#include "input_controller.hpp"
#include "event_manager.hpp"
#include "cube.hpp"


namespace engine {

    struct WindowConfig {
        int width;
        int height;
        const char* title;

        WindowConfig(int width, int height, const char* title)
            : width(width), height(height), title(title) {}
    };
    
    // TODO: Reorder members
    class EngineController {
        private:
            GLFWwindow* window;
            InputController* input_controller;
            EventManager event_manager;
            static EngineController instance; // TODO: Remove this singleton pattern
            static float screen_ratio; // TODO: Make non-static?
            static GLuint gpu_program_id;
            static void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
            static void error_callback(int error, const char* description);
            static GLFWwindow* init_window(WindowConfig window_config);
            static void load_shader(const char* filename, GLuint shader_id);
            static GLuint load_shader_vertex(const char* filename);
            static GLuint load_shader_fragment(const char* filename);
            static GLuint create_gpu_program(GLuint vertex_shader_id, GLuint fragment_shader_id);
            static void load_shaders_from_files();
            static std::string get_executable_directory();
            static void draw();
            static std::vector<std::function<void()>> draw_functions;
            
            /// @brief Ticks the engine
            /// @return True if window should close
            bool update_and_test_should_close();

        public:
            EngineController() = default;

            static EngineController start_engine(WindowConfig window_config);
            static EventManager& get_events(); // TODO: Remove this static method
            static InputController* get_input(); // TODO: Remove this static method

            void hand_over_control();
            void add_drawable(Cube* drawable);
            EventManager& events() { return this->event_manager; }
            GLFWwindow* get_window();
            InputController* input();
            float get_screen_ratio() { return screen_ratio; }
            void set_screen_ratio(float ratio) { screen_ratio = ratio; }
            GLuint get_gpu_program_id() { return gpu_program_id; }
    };
}
