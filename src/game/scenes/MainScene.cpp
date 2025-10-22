#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>

using engine::SceneRoot;
using engine::Camera;
using engine::VObjectConfig;
using engine::TransformBuilder;
using engine::math::Quaternion;
using engine::Vec3;
using namespace game::components;

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        root
            .vobject(VObjectConfig()
                .component(new PlayerController())
                .child(VObjectConfig()
                    .transform(TransformBuilder()
                        .position(Vec3(0.0f, 2.0f, 5.0f))
                        .rotation(Quaternion::fromXRotation(-0.3)))
                    .component(new Camera())
                )
            ).vobject(VObjectConfig()
                .component(new BunnyDrawable())
            );
    }
}
