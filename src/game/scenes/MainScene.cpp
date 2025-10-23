#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>
#include "../temp_globals.hpp"
#include <engine/vobject/components/Trajectory.hpp>
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
                .child(
                    VObjectConfig()
                        .transform(TransformBuilder()
                            .position(Vec3(0.0f, player_height, 0.0f)))
                        .component(main_camera)
                )
            )
            .vobject(VObjectConfig()  // Root VObject for all planets
                .transform(TransformBuilder()
                    .position(Vec3(0.0f, 0.0f, 0.0f)))  // Center of solar system
                .child(VObjectConfig()  // Central sun/star
                    .transform(TransformBuilder()
                        .position(Vec3(100.0f, 0.0f, 0.0f))
                        .scale(2.0f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                )
                .child(VObjectConfig()  // Planet 1 - small orbiting planet
                    .transform(TransformBuilder()
                        .position(Vec3(100.0f, 0.0f, 0.0f))
                        .scale(0.1f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(engine::Curve({
                        Vec3(100.0f, 0.0f, 0.0f),
                        Vec3(-100.0f, 0.0f, 55.2f),
                        Vec3(55.2f, 0.0f, 100.0f),
                        Vec3(100.0f, 0.0f, 0.0f)       // End point (same as start)
                    }), 100.0f))
                )
                .child(VObjectConfig()  // Planet 2 - small rock
                    .transform(TransformBuilder()
                        .position(Vec3(-80.0f, 5.0f, 20.0f))   // left-front, slightly up
                        .scale(0.5f * planet_scale))         // small rock
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(engine::Curve({
                        Vec3(-80.0f, 5.0f, 20.0f),
                        Vec3(-40.0f, 25.0f, 60.0f),
                        Vec3(10.0f, 5.0f, 80.0f),
                        Vec3(-80.0f, 5.0f, 20.0f)            // End point (same as start)
                    }), 140.0f))
                )
                .child(VObjectConfig()  // Planet 3 - distant planet
                    .transform(TransformBuilder()
                        .position(Vec3(0.0f, -20.0f, -120.0f)) // far back, below
                        .scale(0.5f * planet_scale))          // big planet
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(engine::Curve({
                        Vec3(0.0f, -20.0f, -120.0f),
                        Vec3(-60.0f, -40.0f, -80.0f),
                        Vec3(60.0f, -10.0f, -40.0f),
                        Vec3(0.0f, -20.0f, -120.0f)
                    }), 220.0f))
                )
                .child(VObjectConfig()  // Planet 4 - medium planet
                    .transform(TransformBuilder()
                        .position(Vec3(50.0f, 10.0f, 60.0f))
                        .scale(0.8f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(engine::Curve({
                        Vec3(50.0f, 10.0f, 60.0f),
                        Vec3(80.0f, 30.0f, 20.0f),
                        Vec3(40.0f, 5.0f, -20.0f),
                        Vec3(50.0f, 10.0f, 60.0f)
                    }), 110.0f))
                )
                .child(VObjectConfig()  // Planet 5 - high orbit planet
                    .transform(TransformBuilder()
                        .position(Vec3(-30.0f, 40.0f, 90.0f))
                        .scale(1.0f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(engine::Curve({
                        Vec3(-30.0f, 40.0f, 90.0f),
                        Vec3(-10.0f, 80.0f, 60.0f),
                        Vec3(30.0f, 60.0f, 30.0f),
                        Vec3(-30.0f, 40.0f, 90.0f)
                    }), 180.0f))
                )
            );
    }
}
