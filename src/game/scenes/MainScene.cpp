#include "MainScene.hpp"
#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/curves/BezierCurve.hpp>
#include <engine/math/curves/CircularCurve.hpp>
#include <engine/math/curves/PieceWiseBezierCurve.hpp>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>
#include <game/components/player/CameraController.hpp>
#include <game/components/player/PlayerController.hpp>
#include <game/components/SpaceshipController.hpp>
#include <game/components/Drawables/Stars.hpp>
#include <game/components/Drawables/Particles.hpp>

using namespace engine;
using namespace game::components;

VObjectConfig Player(PlayerController*& player_ref, Camera* main_camera, float height, std::vector<PlanetInfo*> planets) {
    // Create walker component first and then the humanoid which will forward inputs to it.
    WalkerController* walker = new WalkerController(planets);
    // TODO: add spaceship controller pointer (nullptr for now)
    PlayerController* controller = new PlayerController(main_camera, walker, NULL);
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

VObjectConfig SpaceShipPlayer(SpaceshipController*& controller_ref, Camera* first_person_camera) {
    SpaceshipController* controller = new SpaceshipController(first_person_camera, 0.0f);
    controller_ref = controller;
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(220.0f, 50.0f, 0.0f)))
        .component(controller)
        .component(new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg")))
        .child(VObjectConfig()
            .component(first_person_camera)
        );
}

VObjectConfig Planet(PlanetInfo* planet_info) {
    return VObjectConfig()
        .component(planet_info)
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 0.0f, 0.0f))
            )
            .component(new ObjDrawable(std::string("mars.obj"), std::string("mars.jpg"), EngineController::ShaderType::Planet))
        );
}

VObjectConfig SpaceshipObj() {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 0.0f, 0.0f))
            .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
        )
        .component(new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg")));
}

VObjectConfig Enemy(PlayerController* player_ref, std::vector<PlanetInfo*> planets) {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(50.0f, 220.0f, 0.0f)))
        .component(new GroundEnemyController(player_ref, planets))
        .child(SpaceshipObj());
}

VObjectConfig SkyBox() {
    return VObjectConfig()
        // .transform(TransformBuilder()
        //     .scale(10000.0f))
        .component(new Stars(5000));
}

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        const float player_height = 1.8f;
        const float planet_model_normalize = 1.0f; // Very precise estimate

        Camera* humanoid_camera = new Camera();
        Camera* spaceship_third_person_camera = new Camera();
        Camera* spaceship_first_person_camera = new Camera();
        Camera::set_main(spaceship_third_person_camera);
        PlayerController* player_ref = nullptr;
        std::vector<PlanetInfo*> planets;
        planets.push_back(new PlanetInfo(55.0e12f, 200.0f));
        planets.push_back(new PlanetInfo(25.0e12f, 50.0f));

        SpaceshipController* spaceship_controller_ref = nullptr;

        root
            // Spaceship player
            .vobject(VObjectConfig().component(spaceship_third_person_camera))
            .vobject(SpaceShipPlayer(spaceship_controller_ref, spaceship_first_person_camera))
            .vobject(VObjectConfig().component(new CameraController(spaceship_controller_ref, spaceship_third_person_camera)))
            // HumanoidPlayer
            .vobject(SkyBox())
            .vobject(VObjectConfig().component(new Particles(100)))
            .vobject(Player(player_ref, humanoid_camera, player_height, planets))
            // ensure the camera component is attached to a VObject so Camera::get_vobject() is valid
            //.vobject(Enemy(player_ref, planets))
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(Planet(planets[0])  // Central star
                    .transform(TransformBuilder()
                        .scale(200.0f * planet_model_normalize))
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
