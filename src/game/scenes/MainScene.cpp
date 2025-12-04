#include "MainScene.hpp"
#include <engine>
#include <cmath>
#include <vector>
#include <game/components/include.hpp>
#include <game/instantiators/Enemy.hpp>
#include <game/scenes/MainScene_vars.hpp>
#include <game/instantiators/EnemyShip.hpp>
#include <game/instantiators/SpaceshipObj.hpp>
#include <engine/math/random.hpp>

using namespace engine;
using namespace game::components;
using namespace game::instantiators;

std::vector<VObjectConfig> Player() {
    const float height = 1.8f;

    Camera* humanoid_camera = new Camera();
    Camera* ship_camera = new Camera();

    KinematicBody* kinematic = new KinematicBody();
    ObjDrawable* ship_drawable;
    VObjectConfig spaceship_obj = SpaceshipObj(ship_drawable);
    Gravity* gravity = new Gravity(kinematic);

    engine::PointCollider* point_collider = new engine::PointCollider();
    engine::CylinderCollider* cylinder_collider = new engine::CylinderCollider(height, 0.5f);
    engine::CylinderCollider* ship_collider = new engine::CylinderCollider(2.0f, 1.0f);
    AngularVelocity* angular_velocity = new AngularVelocity();
    
    SpaceshipController* ship_ctl = new SpaceshipController(kinematic, angular_velocity);
    ShipCameraController* ship_cam_ctl = new ShipCameraController(ship_camera, humanoid_camera);
    PlayerShipController* player_ship = new PlayerShipController(ship_ctl, ship_cam_ctl, ship_drawable, ship_collider);
    WalkerController* walker = new WalkerController(kinematic, gravity, point_collider);
    HumanoidPlayerController* humanoid_controller = new HumanoidPlayerController(humanoid_camera, walker, angular_velocity, cylinder_collider);
    PlayerController* player_ctl = new PlayerController(humanoid_controller, player_ship, humanoid_camera, ship_camera);
    game::scenes::main_scene::player = player_ctl;

    ThrustParticles* thrust_particles = new ThrustParticles(ship_ctl);

    std::vector<VObjectConfig> vobjs;

    VObjectConfig player_vobj = VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 220.0f, 0.0f)))
        .component(walker)
        .component(humanoid_controller)
        .component(player_ship)
        .component(point_collider)
        .component(cylinder_collider)
        .component(ship_ctl)
        .component(player_ctl)
        .component(gravity)
        .component(angular_velocity)
        .component(kinematic)
        .component(ship_cam_ctl)
        .component(thrust_particles)
        .child(spaceship_obj)
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .rotation(Quaternion::from_x_rotation(-M_PI)))
            .component(ship_collider)
        )
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, height, 0.0f)))
            .component(humanoid_camera)
        );

    vobjs.push_back(player_vobj);
    //vobjs.push_back(VObjectConfig().component(new ThrustParticles(ship_ctl)));
    vobjs.push_back(VObjectConfig().component(ship_camera));

    return vobjs;
}

VObjectConfig Planet(PlanetInfo* planet_info, float planet_radius) {
    return VObjectConfig()
        .component(planet_info)
        .component(planet_info->get_sphere_collider())
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .scale(planet_radius)
            )
            .component(new ObjDrawable(std::string("mars.obj"), EngineController::ShaderType::Planet))
        );
}

VObjectConfig FlyingSatellite() {
    return VObjectConfig()
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .scale(0.002f)
            )
            .component(new ObjDrawable(std::string("10477_Satellite_v1_L3.obj"), std::string("10477_Satellite_v1_Diffuse.jpg")))
            .component(new Trajectory(std::make_unique<engine::PieceWiseBezierCurve>(
                std::vector<engine::BezierCurve>{
                    // Two cubic Bezier curves forming a closed loop around origin
                    // Curve 1: from +X to -X (upper arc)
                    engine::BezierCurve({
                        Vec3(280.0f, 260.0f, 0.0f),
                        Vec3(280.0f, 300.0f, 160.0f),
                        Vec3(-280.0f, 220.0f, 160.0f),
                        Vec3(-280.0f, 260.0f, 0.0f)
                    }),
                    // Curve 2: from -X back to +X (lower arc)
                    engine::BezierCurve({
                        Vec3(-280.0f, 260.0f, 0.0f),
                        Vec3(-280.0f, 300.0f, -160.0f),
                        Vec3(280.0f, 220.0f, -160.0f),
                        Vec3(280.0f, 260.0f, 0.0f)
                    })
                }
            ), 20.0f, 0.0f))
        );
}

VObjectConfig PlanetWithEnemy(PlanetInfo* planet_info, float planet_radius) {
    return Planet(planet_info, planet_radius)
        .component(new GroundEnemySpawner(planet_info));
}

VObjectConfig SkyBox() {
    return VObjectConfig()
        // .transform(TransformBuilder()
        //     .scale(10000.0f))
        .component(new Stars(5000));
}

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {

        std::vector<PlanetInfo*>& planets = scenes::main_scene::planets;
        planets.push_back(new PlanetInfo(70.0e12f, MAIN_PLANET_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_1_RADIUS/MAIN_PLANET_RADIUS, PLANET_1_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_2_RADIUS/MAIN_PLANET_RADIUS, PLANET_2_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_3_RADIUS/MAIN_PLANET_RADIUS, PLANET_3_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_4_RADIUS/MAIN_PLANET_RADIUS, PLANET_4_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_5_RADIUS/MAIN_PLANET_RADIUS, PLANET_5_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_6_RADIUS/MAIN_PLANET_RADIUS, PLANET_6_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_7_RADIUS/MAIN_PLANET_RADIUS, PLANET_7_RADIUS));
        planets.push_back(new PlanetInfo(70.0e12f*PLANET_8_RADIUS/MAIN_PLANET_RADIUS, PLANET_8_RADIUS));

        root
            .vobjects(Player())
            .vobject(SkyBox())
            .vobject(VObjectConfig()  // Root VObject for all planets
                .child(FlyingSatellite())
                .child(PlanetWithEnemy(planets[0], MAIN_PLANET_RADIUS))  // Central planet
                .child(PlanetWithEnemy(planets[1], PLANET_1_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.2f))
                )
                .child(PlanetWithEnemy(planets[2], PLANET_2_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        700.0f                      // Radius
                    ), 0.002f, 0.8f))
                )
                .child(PlanetWithEnemy(planets[3], PLANET_3_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.7f))
                )
                .child(PlanetWithEnemy(planets[4], PLANET_4_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        1400.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(PlanetWithEnemy(planets[5], PLANET_5_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.4f))
                )
                .child(PlanetWithEnemy(planets[6], PLANET_6_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2100.0f                      // Radius
                    ), 0.002f, 0.9f))
                )
                .child(PlanetWithEnemy(planets[7], PLANET_7_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2800.0f                      // Radius
                    ), 0.002f, 0.5f))
                )
                .child(PlanetWithEnemy(planets[8], PLANET_8_RADIUS)  // Tilted circular orbit
                    .component(new Trajectory(std::make_unique<engine::CircularCurve>(
                        Vec3(0.0f, 0.0f, 0.0f),      // Center at origin
                        Vec3(0.2f, 1.0f, 0.1f),      // Tilted normal vector
                        2800.0f                      // Radius
                    ), 0.002f, 0.7f))
                )
            );
    }

    // Restart scene

    void MainScene::restart() {

        constexpr float DISTANCE_FROM_SURFACE = 2.0f;

        if (game::scenes::main_scene::player) {
            float theta = engine::random_float(0.0f, 2.0f * M_PI);
            float phi = engine::random_float(0.0f, M_PI);

            Vec3 dir;
            dir.x = std::sin(phi) * std::cos(theta);
            dir.y = std::sin(phi) * std::sin(theta);
            dir.z = std::cos(phi);

            Vec3 relative_spawn_pos = dir * (MAIN_PLANET_RADIUS + DISTANCE_FROM_SURFACE);
            game::scenes::main_scene::player->get_vobject()->transform().set_world_position(relative_spawn_pos);
        }
    }
}

