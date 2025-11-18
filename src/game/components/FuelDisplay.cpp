#include "FuelDisplay.hpp"
#include <engine/EngineController.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <sstream>
#include <iomanip>

namespace game::components {
    FuelDisplay::FuelDisplay(SpaceshipController* spaceship_controller)
        : spaceship_controller(spaceship_controller), hud_vao(0), hud_vbo(0), initialized(false) {}
    
    void FuelDisplay::draw() {
        if (!spaceship_controller) return;
        
        render_simple_hud();
    }
    
    void FuelDisplay::render_simple_hud() {
        if (!initialized) {
            // Initialize VAO/VBO for fuel bar
            glGenVertexArrays(1, &hud_vao);
            glGenBuffers(1, &hud_vbo);
            initialized = true;
        }
        
        float fuel = spaceship_controller->get_fuel();
        float max_fuel = spaceship_controller->get_max_fuel();
        float percentage = fuel / max_fuel;
        
        // Save OpenGL state
        GLboolean depth_test_enabled;
        glGetBooleanv(GL_DEPTH_TEST, &depth_test_enabled);
        GLboolean blend_enabled;
        glGetBooleanv(GL_BLEND, &blend_enabled);
        GLint current_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        
        // Disable depth test and enable blending for HUD
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use no shader program for simple rendering
        glUseProgram(0);
        
        // Get window size
        GLFWwindow* window = engine::EngineController::get_instance()->get_window();
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        
        // Setup orthographic projection for 2D HUD
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, window_width, 0, window_height, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        // Draw fuel bar background (dark gray)
        float bar_x = 20.0f;
        float bar_y = window_height - 40.0f;
        float bar_width = 200.0f;
        float bar_height = 20.0f;
        
        glBegin(GL_QUADS);
        glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
        glVertex2f(bar_x, bar_y);
        glVertex2f(bar_x + bar_width, bar_y);
        glVertex2f(bar_x + bar_width, bar_y + bar_height);
        glVertex2f(bar_x, bar_y + bar_height);
        glEnd();
        
        // Draw fuel bar fill (colored based on fuel level)
        float fill_width = bar_width * percentage;
        glm::vec3 fuel_color;
        if (percentage > 0.5f) {
            fuel_color = glm::vec3(0.0f, 1.0f, 0.0f); // Green
        } else if (percentage > 0.25f) {
            fuel_color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
        } else {
            fuel_color = glm::vec3(1.0f, 0.0f, 0.0f); // Red
        }
        
        glBegin(GL_QUADS);
        glColor4f(fuel_color.r, fuel_color.g, fuel_color.b, 0.9f);
        glVertex2f(bar_x, bar_y);
        glVertex2f(bar_x + fill_width, bar_y);
        glVertex2f(bar_x + fill_width, bar_y + bar_height);
        glVertex2f(bar_x, bar_y + bar_height);
        glEnd();
        
        // Draw border
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex2f(bar_x, bar_y);
        glVertex2f(bar_x + bar_width, bar_y);
        glVertex2f(bar_x + bar_width, bar_y + bar_height);
        glVertex2f(bar_x, bar_y + bar_height);
        glEnd();
        
        // Restore matrices
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        // Restore OpenGL state
        if (depth_test_enabled) {
            glEnable(GL_DEPTH_TEST);
        }
        if (!blend_enabled) {
            glDisable(GL_BLEND);
        }
        glUseProgram(current_program);
    }
} // namespace game::components
