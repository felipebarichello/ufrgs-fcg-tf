#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <game/components/drawables/BunnyDrawable.hpp>
#include "../temp_globals.hpp"
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/BezierCurve.hpp>
#include <engine/math/CircularCurve.hpp>
#include <engine/math/PieceWiseBezierCurve.hpp>
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
                    .position(Vec3(100.0f, 0.0f, 0.0f)))
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
                .child(VObjectConfig()  // Planet with dynamic piecewise Bezier curve
                    .transform(TransformBuilder()
                        .scale(0.3f * planet_scale))
                    .component(new ObjDrawable("sphere.obj"))
                    .component(new Trajectory(std::make_unique<engine::PieceWiseBezierCurve>(std::vector<engine::BezierCurve>{
                        // First segment: dramatic spiral outward and up
                        engine::BezierCurve(std::vector<Vec3>{
                            Vec3(30.0f, 0.0f, 0.0f),       // Start point
                            Vec3(45.0f, 25.0f, 20.0f),     // Control point 1 - high and forward
                            Vec3(15.0f, 50.0f, 35.0f),     // Control point 2 - very high with twist
                            Vec3(-20.0f, 35.0f, 15.0f)     // End point - back and left
                        }),
                        // Second segment: swooping dive with twist
                        engine::BezierCurve(std::vector<Vec3>{
                            Vec3(-20.0f, 35.0f, 15.0f),    // Start point (same as previous end)
                            Vec3(-50.0f, 15.0f, -10.0f),   // Control point 1 - far left and down
                            Vec3(-60.0f, -20.0f, -30.0f),  // Control point 2 - deep dive
                            Vec3(-25.0f, -45.0f, -15.0f)   // End point - coming back up
                        }),
                        // Third segment: figure-8 loop
                        engine::BezierCurve(std::vector<Vec3>{
                            Vec3(-25.0f, -45.0f, -15.0f),  // Start point
                            Vec3(10.0f, -60.0f, 5.0f),     // Control point 1 - loop around
                            Vec3(40.0f, -35.0f, 25.0f),    // Control point 2 - completing loop
                            Vec3(35.0f, -10.0f, 10.0f)     // End point - rising
                        }),
                        // Fourth segment: high altitude curve with dramatic descent
                        engine::BezierCurve(std::vector<Vec3>{
                            Vec3(35.0f, -10.0f, 10.0f),    // Start point
                            Vec3(55.0f, 15.0f, 40.0f),     // Control point 1 - soaring high
                            Vec3(50.0f, 30.0f, 25.0f),     // Control point 2 - peak altitude
                            Vec3(30.0f, 0.0f, 0.0f)        // End point - back to start with smooth landing
                        }),
                        // Fifth segment: bonus corkscrew for extra dynamics
                        engine::BezierCurve(std::vector<Vec3>{
                            Vec3(30.0f, 0.0f, 0.0f),       // Start point (same as very first)
                            Vec3(25.0f, -15.0f, -25.0f),   // Control point 1 - diving down
                            Vec3(45.0f, -25.0f, -15.0f),   // Control point 2 - corkscrew motion
                            Vec3(30.0f, 0.0f, 0.0f)        // End point - complete the loop
                        })
                    }), 15.0f))
                )
            );
    }
}
