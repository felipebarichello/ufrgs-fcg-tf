#include "MainScene.hpp"
#include <game/components/player/HumanoidPlayerController.hpp>
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
            .position(Vec3(0.0f, 200.0f, 50.0f)))
        .component(new HumanoidPlayerController(main_camera, planets))
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, height, 0.0f)))
            .component(main_camera)
        );
        // .child(VObjectConfig()
        //     .transform(TransformBuilder()
        //         .position(Vec3(0.0f, -0.0f, 0.0f))
        //     )
        //     .component(new ObjDrawable("bunny.obj"))
        // );
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
        planets.push_back(new PlanetInfo(20.0e12f, 50.0f));

        root
            .vobject(Player(main_camera, player_height, planets))
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(Planet(planets[0])  // Central star
                    .transform(TransformBuilder()
                        .scale(200.0f * planet_model_normalize))
                    .child(VObjectConfig()
                        .transform(TransformBuilder()
                            .position(Vec3(0.0f, 11.0f, 0.0f)))
                        .component(new ObjDrawable("bunny.obj"))
                    )
                )
                .child(Planet(planets[1])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(50.0f * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f))
                )
            );
    }
}
