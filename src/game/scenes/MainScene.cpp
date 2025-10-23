#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>
#include "../temp_globals.hpp"
#include <cmath>


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

        Camera* main_camera = new Camera();
        Camera::set_main(main_camera);

        root
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 20.5f, 0.0f)))
                .component(player_controller)
                .child(VObjectConfig()
                    .transform(TransformBuilder()
                        .rotation(Quaternion::fromEulerZYX(Vec3(0.0f, -M_PI, 0.0f))))
                    .component(main_camera)
                )
            )
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 0.0f, 0.0f))
                    .scale(1.0f))
                .component(new ObjDrawable("sphere.obj"))
            );
    }
}
