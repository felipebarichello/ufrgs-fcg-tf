#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>
#include "../temp_globals.hpp"

using engine::SceneRoot;
using engine::Camera;
using engine::VObjectConfig;
using engine::TransformBuilder;
using engine::math::Quaternion;
using engine::Vec3;
using namespace game::components;

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        PlayerController* player_controller = new PlayerController();
        temp::player_controller = player_controller;

        root
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 0.0f, 3.0f)))
                .component(player_controller)
                .child(VObjectConfig()
                    .component(new Camera())
                )
            ).vobject(VObjectConfig()
                .component(new BunnyDrawable())
            );
    }
}
