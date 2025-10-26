#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/curves/BezierCurve.hpp>
#include <engine/math/curves/CircularCurve.hpp>
#include <engine/math/curves/PieceWiseBezierCurve.hpp>
#include <cmath>
#include <vector>
#include <game/components/PlanetInfo.hpp>

using namespace engine;
using namespace game::components;

VObjectConfig Player(Camera* main_camera, float height, std::vector<PlanetInfo*> planets) {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 0.0f, 240.0f)))
        .component(new PlayerController(main_camera, planets))
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 18.0f, 0.0f))
                .scale(Vec3(10.0f)))
            .component(new ObjDrawable("bunny.obj"))
        )
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, height, 0.0f)))
            .component(main_camera)
        );
}

VObjectConfig Planet(PlanetInfo* planet_info) {
    return VObjectConfig()
        .component(planet_info)
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, -10.0f, 0.0f))
            )
            .component(new ObjDrawable("sphere.obj"))
        );
}

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        const float player_height = 1.8f;
        const float planet_model_normalize = 1.0f / 10.0f; // Very precise estimate

        Camera* main_camera = new Camera();
        Camera::set_main(main_camera);

        std::vector<PlanetInfo*> planets;
        planets.push_back(new PlanetInfo(50.0e12f, 200.0f));
        planets.push_back(new PlanetInfo(0.0f, 50.0f));
        planets.push_back(new PlanetInfo(0.0f, 16.0f));
        planets.push_back(new PlanetInfo(0.0f, 24.0f));
        planets.push_back(new PlanetInfo(0.0f, 24.0f));

        root
            .vobject(Player(main_camera, player_height, planets))
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(Planet(planets[0])  // Central star
                    .transform(TransformBuilder()
                        .scale(200.0f * planet_model_normalize))
                )
                .child(Planet(planets[1])  // Planet 4 - tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(50.0f * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        600.0f                       // Radius
                    ), 0.01f))
                )
                .child(Planet(planets[2])  // Planet 5 - outer circular orbit
                    .transform(TransformBuilder()
                        .scale(16.0f * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(1.2f, 0.0f, 0.1f),      // Tilted normal vector
                        30.0f                         // Radius
                    ), 0.5f))
                )
                .child(Planet(planets[3])  // Planet 5 - outer circular orbit
                    .transform(TransformBuilder()
                        .scale(24.0f * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(1.0f, 1.0f, 1.0f),      // Tilted normal vector
                        30.0f                         // Radius
                    ), 0.6f))
                )
                .child(Planet(planets[4])  // Planet with dynamic piecewise Bezier curve
                    .transform(TransformBuilder()
                        .scale(24.0f * planet_model_normalize))
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
