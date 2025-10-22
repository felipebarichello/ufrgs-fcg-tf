#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine>
#include <game/components/drawables/Cube.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>

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
                .child(VObjectConfig()
                    .component(new BunnyDrawable())
                )
            );
    }
}
