#include "MainScene.hpp"
#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/curves/BezierCurve.hpp>
#include <engine/math/curves/CircularCurve.hpp>
#include <engine/math/curves/PieceWiseBezierCurve.hpp>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>

using namespace engine;
using namespace game::components;

VObjectConfig Player(HumanoidPlayerController*& player_ref, Camera* main_camera, float height, std::vector<PlanetInfo*> planets) {
    // Create walker component first and then the humanoid which will forward inputs to it.
    WalkerController* walker = new WalkerController(planets);
    HumanoidPlayerController* controller = new HumanoidPlayerController(main_camera, walker);
    player_ref = controller;

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 220.0f, 50.0f)))
        .component(walker)
        .component(controller)
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

VObjectConfig BunnyObj() {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 0.95f, 0.0f))
            .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
        )
        .component(new ObjDrawable("bunny.obj"));
}

VObjectConfig Enemy(HumanoidPlayerController* player_ref, std::vector<PlanetInfo*> planets) {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(50.0f, 220.0f, 0.0f)))
        .component(new GroundEnemyController(player_ref, planets))
        .child(BunnyObj());
}

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        const float player_height = 1.8f;
        const float planet_model_normalize = 1.0f / 10.0f; // Very precise estimate

        Camera* main_camera = new Camera();
        Camera::set_main(main_camera);
        HumanoidPlayerController* player_ref = nullptr;

        std::vector<PlanetInfo*> planets;
        planets.push_back(new PlanetInfo(50.0e12f, 200.0f));
        planets.push_back(new PlanetInfo(20.0e12f, 50.0f));

        root
            .vobject(Player(player_ref, main_camera, player_height, planets))
            .vobject(Enemy(player_ref, planets))
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(Planet(planets[0])  // Central star
                    .transform(TransformBuilder()
                        .scale(200.0f * planet_model_normalize))
                    .child(VObjectConfig()
                        .transform(TransformBuilder()
                            .position(Vec3(0.0f, 10.0f, 0.0f)))
                        .child(BunnyObj())
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
