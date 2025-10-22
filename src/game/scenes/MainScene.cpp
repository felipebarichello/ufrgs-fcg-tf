#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/VObject.hpp>

using engine::SceneRoot;

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        using namespace game::components;

        root
            .vobject(VObjectConfig()
                .component(new PlayerController())
                .component(new Controller2())
                .component(new Controller2()) // TODO: That should be illegal
                .child(VObjectConfig()
                    .component(new Controller2())));
    }
}
