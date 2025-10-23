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
        const float player_height = 1.8f;
        const float default_planet_radius = 20.0f; // Very precise estimate
        const float planet_radius = 20.0f;
        const float planet_scale = planet_radius / default_planet_radius;

        Camera* main_camera = new Camera();
        Camera::set_main(main_camera);

        PlayerController* player_controller = new PlayerController(main_camera, planet_radius);
        temp::player_controller = player_controller;

        root
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 100.0f, 0.0f)))
                .component(player_controller)
                .child(VObjectConfig()
                    .transform(TransformBuilder()
                        .position(Vec3(0.0f, player_height, 0.0f)))
                    .component(main_camera)
                )
            )
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(100.0f, 0.0f, 0.0f))
                    .scale(1.0f * planet_scale))
                .component(new ObjDrawable("sphere.obj"))
            )
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 0.0f, 0.0f))
                    .scale(0.10f))
                .component(new ObjDrawable("sphere.obj"))
            );
    }
}
