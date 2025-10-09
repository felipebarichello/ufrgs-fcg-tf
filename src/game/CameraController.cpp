#include "CameraController.hpp"

namespace game {
    CameraController::CameraController() {
        std::cout << "CameraController created" << std::endl;
    }

    CameraController::~CameraController() {
        std::cout << "CameraController destroyed" << std::endl;
    }

    void CameraController::Start() {
        std::cout << "CameraController started" << std::endl;
    }

    void CameraController::Update() {
        std::cout << "CameraController updated" << std::endl;
    }
}
