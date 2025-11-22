#include "MainScene.hpp"
#include <engine>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>

using namespace engine;
using namespace game::components;

VObjectConfig Player(HumanoidPlayerController*& player_ref, Camera* main_camera, float height, std::vector<PlanetInfo*> planets, SpaceshipController*& ship_controller) {
    KinematicBody* kinematic = new KinematicBody();
    ObjDrawable* ship_drawable = new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg"));
    Gravity* gravity = new Gravity(kinematic, planets);

    engine::PointCollider* point_collider = new engine::PointCollider();
    engine::CylinderCollider* cylinder_collider = new engine::CylinderCollider(height, 0.5f);
    engine::CylinderCollider* spaceship_collider = new engine::CylinderCollider(2.0f, 1.0f);

    ship_controller = new SpaceshipController(kinematic, planets, ship_drawable, spaceship_collider);

    // Create walker component first and then the humanoid which will forward inputs to it.
    WalkerController* walker = new WalkerController(kinematic, planets, point_collider);

    // TODO: add spaceship controller pointer (nullptr for now)
    HumanoidPlayerController* humanoid_controller = new HumanoidPlayerController(main_camera, walker, cylinder_collider);
    player_ref = humanoid_controller;

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 180.0f, 100.0f)))
        .component(walker)
        .component(humanoid_controller)
        .component(ship_controller)
        .component(ship_drawable)
        .component(point_collider)
        .component(cylinder_collider)
        .component(spaceship_collider)
        .component(gravity)
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
            .component(new ObjDrawable(std::string("mars.obj"), EngineController::ShaderType::Planet))
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
            .position(Vec3(0.0f, MAIN_PLANET_RADIUS, 0.0f)))
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
        planets.push_back(new PlanetInfo(55.0e12f, MAIN_PLANET_RADIUS));
        planets.push_back(new PlanetInfo(35.0e12f, PLANET_1_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_2_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_3_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_4_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_5_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_6_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_7_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_8_RADIUS));

        SpaceshipController* spaceship_controller_ref = nullptr;

        root
            .vobject(Player(player_ref, humanoid_camera, player_height, planets, spaceship_controller_ref))
            .vobject(VObjectConfig().component(spaceship_camera))
            .vobject(VObjectConfig().component(new SpaceshipCameraController(spaceship_controller_ref, spaceship_camera)))
            // HumanoidPlayer
            .vobject(SkyBox())
            .vobject(VObjectConfig().component(new PlayerSwitcherController(player_ref, spaceship_controller_ref, humanoid_camera, spaceship_camera)))
            // ensure the camera component is attached to a VObject so Camera::get_vobject() is valid

            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(Enemy(player_ref, planets))
                .child(Planet(planets[0])  // Central star
                    .transform(TransformBuilder()
                        .scale(MAIN_PLANET_RADIUS * planet_model_normalize))
                    //.child(Enemy(player_ref, planets))
                )
                .child(Planet(planets[1])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_1_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.2f))
                )
                .child(Planet(planets[2])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_2_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.8f))
                )
                .child(Planet(planets[3])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_3_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.7f))
                )
                .child(Planet(planets[4])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_4_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(Planet(planets[5])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_5_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(Planet(planets[6])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_6_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.9f))
                )
                .child(Planet(planets[7])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_7_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2800.0f                      // Radius
                    ), 0.002f, 0.5f))
                )
                .child(Planet(planets[8])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_8_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2800.0f                      // Radius
                    ), 0.002f, 0.7f))
                )
            );
    }
}
