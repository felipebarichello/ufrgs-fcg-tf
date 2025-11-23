#include "MainScene.hpp"
#include <engine>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>
#include <game/instantiators/Enemy.hpp>
#include <game/scenes/MainScene_vars.hpp>

using namespace engine;
using namespace game::components;
using namespace game::instantiators;

VObjectConfig Player(Camera* main_camera, float height, Camera* humanoid_camera, Camera* ship_camera, SpaceshipController*& ship_controller) {
    KinematicBody* kinematic = new KinematicBody();
    ObjDrawable* ship_drawable = new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg"));
    Gravity* gravity = new Gravity(kinematic);

    engine::PointCollider* point_collider = new engine::PointCollider();
    engine::CylinderCollider* cylinder_collider = new engine::CylinderCollider(height, 0.5f);
    engine::CylinderCollider* ship_collider = new engine::CylinderCollider(2.0f, 1.0f);
    AngularVelocity* angular_velocity = new AngularVelocity();

    ship_controller = new SpaceshipController(kinematic, angular_velocity, ship_drawable, ship_collider);
    WalkerController* walker = new WalkerController(kinematic, gravity, point_collider);
    HumanoidPlayerController* humanoid_controller = new HumanoidPlayerController(main_camera, walker, cylinder_collider);
    game::scenes::main_scene::player = humanoid_controller;

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 180.0f, 100.0f)))
        .component(walker)
        .component(humanoid_controller)
        .component(ship_controller)
        .component(point_collider)
        .component(cylinder_collider)
        .component(ship_collider)
        .component(new PlayerSwitcherController(humanoid_controller, ship_controller, humanoid_camera, ship_camera))
        .component(gravity)
        .component(angular_velocity)
        .component(kinematic)
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, -0.5f, 0.0f)))
            .component(ship_drawable)
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
        .component(planet_info->get_sphere_collider())
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 0.0f, 0.0f))
            )
            .component(new ObjDrawable(std::string("mars.obj"), EngineController::ShaderType::Planet))
        );
}

VObjectConfig PlanetWithEnemy(PlanetInfo* planet_info) {
    return Planet(planet_info)
        .component(new GroundEnemySpawner(planet_info));
}

VObjectConfig SpaceshipObj() {
    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 0.0f, 0.0f))
            .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
        )
        .component(new ObjDrawable(std::string("spaceship.obj"), std::string("spaceship.jpg")));
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
        Camera* ship_camera = new Camera();
        Camera::set_main(humanoid_camera);

        std::vector<PlanetInfo*>& planets = scenes::main_scene::planets;
        planets.push_back(new PlanetInfo(55.0e12f, MAIN_PLANET_RADIUS));
        planets.push_back(new PlanetInfo(35.0e12f, PLANET_1_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_2_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_3_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_4_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_5_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_6_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_7_RADIUS));
        planets.push_back(new PlanetInfo(25.0e12f, PLANET_8_RADIUS));

        SpaceshipController* ship_ref = nullptr;

        root
            .vobject(Player(humanoid_camera, player_height, humanoid_camera, ship_camera, ship_ref))
            .vobject(VObjectConfig().component(new SpaceshipCameraController(ship_ref, ship_camera)))
            .vobject(VObjectConfig().component(ship_camera))
            .vobject(SkyBox())
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(PlanetWithEnemy(planets[0])  // Central planet
                    .transform(TransformBuilder()
                        .scale(MAIN_PLANET_RADIUS * planet_model_normalize))
                )
                .child(PlanetWithEnemy(planets[1])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_1_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.2f))
                )
                .child(PlanetWithEnemy(planets[2])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_2_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.8f))
                )
                .child(PlanetWithEnemy(planets[3])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_3_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.7f))
                )
                .child(PlanetWithEnemy(planets[4])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_4_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(PlanetWithEnemy(planets[5])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_5_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(PlanetWithEnemy(planets[6])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_6_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.9f))
                )
                .child(PlanetWithEnemy(planets[7])  // Tilted circular orbit
                    .transform(TransformBuilder()
                        .scale(PLANET_7_RADIUS * planet_model_normalize))
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2800.0f                      // Radius
                    ), 0.002f, 0.5f))
                )
                .child(PlanetWithEnemy(planets[8])  // Tilted circular orbit
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
