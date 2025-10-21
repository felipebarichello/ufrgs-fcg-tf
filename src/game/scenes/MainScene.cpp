#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>

namespace game::scenes {
    void MainScene::hierarchy(std::vector<VObjectConfig>& root) {
        using namespace game::components;

        root.push_back(
            VObjectConfig()
                .component(new PlayerController())
                .component(new Controller2())
                .component(new Controller2())
        );
    }
}
