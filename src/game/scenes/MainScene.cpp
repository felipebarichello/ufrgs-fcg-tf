#include "MainScene.hpp"
#include <engine>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>
#include <game/components/player/SpaceshipCameraController.hpp>
#include <game/components/player/HumanoidPlayerController.hpp>
#include <game/components/SpaceshipController.hpp>
#include <game/components/player/PlayerSwitcherController.hpp>
#include <game/components/Drawables/Stars.hpp>
#include <game/components/Drawables/SpaceParticles.hpp>

using namespace engine;
using namespace game::components;

VObjectConfig Player(HumanoidPlayerController*& player_ref, Camera* main_camera, float height, std::vector<PlanetInfo*> planets, SpaceshipController*& ship_controller) {
    ObjDrawable* ship_drawable = new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg"));
    
    engine::PointCollider* point_collider = new engine::PointCollider();
    engine::CylinderCollider* cylinder_collider = new engine::CylinderCollider(height, 0.5f);
    ship_controller = new SpaceshipController(ship_drawable);

    // Create walker component first and then the humanoid which will forward inputs to it.
    KinematicBody* kinematic = new KinematicBody();
    WalkerController* walker = new WalkerController(kinematic, planets, point_collider);

    // TODO: add spaceship controller pointer (nullptr for now)
    HumanoidPlayerController* humanoid_controller = new HumanoidPlayerController(main_camera, walker, cylinder_collider);
    player_ref = humanoid_controller;

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 220.0f, 50.0f)))
        .component(walker)
        .component(humanoid_controller)
        .component(ship_controller)
        .component(ship_drawable)
        .component(point_collider)
        .component(cylinder_collider)
        .component(kinematic)
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
        .component(planet_info->get_sphere_collider())
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

VObjectConfig EnemyObj() {
    return VObjectConfig()
        .transform(TransformBuilder()
            .scale(0.03f)
            .position(Vec3(0.0f, -2.0f, 0.0f))
            .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
        )
        .component(new ObjDrawable(std::string("VacuumCleaner/VacuumCleaner.obj"), std::string("spaceship.jpg")));
}

VObjectConfig Enemy(HumanoidPlayerController* player_ref, std::vector<PlanetInfo*> planets) {
    PointCollider* point_collider = new PointCollider();
    CylinderCollider* cylinder_collider = new CylinderCollider(1.0f, 3.0f);
    KinematicBody* kinematic = new KinematicBody();
    WalkerController* walker = new WalkerController(kinematic, planets, point_collider);

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(50.0f, 220.0f, 0.0f)))
        .component(walker)
        .component(point_collider)
        .component(cylinder_collider)
        .component(new GroundEnemyController(walker, cylinder_collider, player_ref))
        .component(kinematic)
        .child(EnemyObj());
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
        Camera* spaceship_camera = new Camera();
        Camera::set_main(humanoid_camera);
        HumanoidPlayerController* player_ref = nullptr;

        std::vector<PlanetInfo*> planets;
        planets.push_back(new PlanetInfo(55.0e12f, 200.0f));
        planets.push_back(new PlanetInfo(35.0e12f, 50.0f));

        SpaceshipController* spaceship_controller_ref = nullptr;

        root
            .vobject(Player(player_ref, humanoid_camera, player_height, planets, spaceship_controller_ref))
            .vobject(VObjectConfig().component(spaceship_camera))
            .vobject(VObjectConfig().component(new SpaceshipCameraController(spaceship_controller_ref, spaceship_camera)))
            // HumanoidPlayer
            .vobject(SkyBox())
            .vobject(VObjectConfig().component(new game::components::PlayerSwitcherController(player_ref, spaceship_controller_ref, humanoid_camera, spaceship_camera)))
            // ensure the camera component is attached to a VObject so Camera::get_vobject() is valid
            .vobject(Enemy(player_ref, planets))
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
