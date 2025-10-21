#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>

namespace game::scenes {
    void MainScene::hierarchy(std::vector<VObjectConfig>& root) {
        using namespace game::components;

        root.push_back(
            VObjectConfig()
                .component(
                    // PlayerController::create("Hello from MainScene")
                    PlayerController::create()
                )
                .component(new Controller2())
        );
    }
}
