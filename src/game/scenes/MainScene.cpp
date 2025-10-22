#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine>

using engine::SceneRoot;
using engine::Camera;

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        using namespace game::components;

        root
            .vobject(VObjectConfig()
                .component(new PlayerController())
                .child(VObjectConfig()
                    .component(new Camera())
                )
            );
    }
}
