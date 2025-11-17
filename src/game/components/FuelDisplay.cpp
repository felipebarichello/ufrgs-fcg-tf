#include "FuelDisplay.hpp"
#include <engine/EngineController.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace game::components {
    FuelDisplay::FuelDisplay(SpaceshipController* spaceship_controller)
        : spaceship_controller(spaceship_controller) {}
    
    void FuelDisplay::Update() {
        if (spaceship_controller) {
            float fuel = spaceship_controller->get_fuel();
            float max_fuel = spaceship_controller->get_max_fuel();
            float percentage = (fuel / max_fuel) * 100.0f;
            
            // Display fuel information in window title
            // This provides simple on-screen text display
            std::ostringstream title;
            title << "FCG - Trabalho Final | Fuel: " 
                  << std::fixed << std::setprecision(1) << fuel 
                  << "/" << max_fuel 
                  << " (" << std::setprecision(0) << percentage << "%)";
            
            GLFWwindow* window = engine::EngineController::get_instance()->get_window();
            glfwSetWindowTitle(window, title.str().c_str());
        }
    }
} // namespace game::components
