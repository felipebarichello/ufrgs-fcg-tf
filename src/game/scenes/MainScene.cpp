#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/BezierCurve.hpp>
#include <engine/math/CircularCurve.hpp>
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

        root
            .vobject(VObjectConfig()
                .transform(TransformBuilder()
                    .position(Vec3(100.0f, 0.0f, 0.0f)))
                .component(new PlayerController(main_camera, planet_radius))
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
                        .position(Vec3(0.0f, 0.0f, 0.0f))  // Center at origin
                        .scale(1.0f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                )
                .child(VObjectConfig()  // Planet 4 - tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(0.3f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        30.0f                         // Radius
                    ), 0.2f))
                )
                .child(VObjectConfig()  // Planet 5 - outer circular orbit
                    .transform(TransformBuilder()
                        .scale(0.2f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(1.2f, 0.0f, 0.1f),      // Tilted normal vector
                        30.0f                         // Radius
                    ), 0.5f))
                )
                .child(VObjectConfig()  // Planet 5 - outer circular orbit
                    .transform(TransformBuilder()
                        .scale(0.3f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(1.0f, 1.0f, 1.0f),      // Tilted normal vector
                        30.0f                         // Radius
                    ), 0.6f))
                )
            );
    }
}
