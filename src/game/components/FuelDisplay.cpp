#include "FuelDisplay.hpp"
#include <engine/EngineController.hpp>
#include <engine/TextRenderer.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace game::components {
    FuelDisplay::FuelDisplay(SpaceshipController* spaceship_controller)
        : spaceship_controller(spaceship_controller), text_renderer(nullptr) {}
    
    FuelDisplay::~FuelDisplay() {
        if (text_renderer) {
            delete text_renderer;
        }
    }
    
    void FuelDisplay::Start() {
        text_renderer = new engine::TextRenderer();
    }
    
    void FuelDisplay::Update() {
        if (spaceship_controller && text_renderer) {
            float fuel = spaceship_controller->get_fuel();
            float max_fuel = spaceship_controller->get_max_fuel();
            float percentage = (fuel / max_fuel) * 100.0f;
            
            // Create fuel display text
            std::ostringstream fuel_text;
            fuel_text << "Fuel: " 
                      << std::fixed << std::setprecision(1) << fuel 
                      << "/" << max_fuel 
                      << " (" << std::setprecision(0) << percentage << "%)";
            
            // Get window size for positioning
            GLFWwindow* window = engine::EngineController::get_instance()->get_window();
            int window_width, window_height;
            glfwGetFramebufferSize(window, &window_width, &window_height);
            
            // Display fuel text in top-left corner with some padding
            float x = 20.0f;
            float y = window_height - 30.0f; // From bottom, so subtract from height
            float scale = 1.0f;
            
            // Choose color based on fuel level
            glm::vec3 color;
            if (percentage > 50.0f) {
                color = glm::vec3(0.0f, 1.0f, 0.0f); // Green
            } else if (percentage > 25.0f) {
                color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
            } else {
                color = glm::vec3(1.0f, 0.0f, 0.0f); // Red
            }
            
            text_renderer->render_text(fuel_text.str(), x, y, scale, color);
        }
    }
} // namespace game::components
