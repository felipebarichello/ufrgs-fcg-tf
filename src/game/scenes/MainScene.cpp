#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
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
            ).vobject(VObjectConfig()
                .component(new BunnyDrawable())
            );
    }
}
