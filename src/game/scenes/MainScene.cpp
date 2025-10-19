#include "MainScene.hpp"
#include <game/components/PlayerComponent.hpp>

namespace game::scenes {
    void MainScene::hierarchy(std::vector<VObjectConfig>& root) {
        // using namespace game::components;

        // root.push_back(VObjectConfig {
        //     .components = {
        //         std::make_unique<PlayerComponentBuilder>()
        //     },
        //     .children = {}
        // });
        
        (void) root;
    }
}
