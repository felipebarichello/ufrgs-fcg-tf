#include "MainScene.hpp"
#include <game/components/PlayerController.hpp>
#include <game/components/CameraController.hpp>
#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Trajectory.hpp>
#include <engine/math/curves/BezierCurve.hpp>
#include <engine/math/curves/CircularCurve.hpp>
#include <engine/math/curves/PieceWiseBezierCurve.hpp>
#include <engine/EngineController.hpp>
#include <cmath>
#include <random>
#include <vector>
#include <memory>
#include <game/components/Drawables/Particles.hpp>
#include <game/components/Drawables/Stars.hpp>

using engine::SceneRoot;
using engine::Camera;
using engine::VObjectConfig;
using engine::TransformBuilder;
using engine::math::Quaternion;
using engine::Vec3;
using namespace game::components;

// Small local Curve wrapper that lets us specify a phase (starting angle) so a circular
// trajectory starts at the desired position without jumping when Trajectory::t == 0.
namespace {
    class PhaseCircularCurve : public engine::Curve {
    public:
        PhaseCircularCurve(const engine::Vec3& center, const engine::Vec3& normal, float radius, float phase)
            : inner(center, normal, radius), phase(phase) {}

        engine::Vec3 get_point(float t) const override {
            float tp = std::fmod(t + phase, 1.0f);
            if (tp < 0.0f) tp += 1.0f;
            return inner.get_point(tp);
        }

        engine::Vec3 get_tangent(float t) const override {
            float tp = std::fmod(t + phase, 1.0f);
            if (tp < 0.0f) tp += 1.0f;
            return inner.get_tangent(tp);
        }

    private:
        engine::CircularCurve inner;
        float phase = 0.0f;
    };
}

VObjectConfig Player(Camera* main_camera, float planet_radius, game::components::PlayerController** out_controller = nullptr) {
    // create player controller explicitly so callers can keep the raw pointer
    auto* pc = new game::components::PlayerController(main_camera, planet_radius);
    if (out_controller) *out_controller = pc;

    return VObjectConfig()
        .transform(TransformBuilder()
            .position(Vec3(0.0f, 0.0f, 200.0f)))
        .component(pc)
        // player model (bunny) as child
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 0.0f, 0.0f))
                .rotation(Quaternion::from_axis_angle(Vec3(0.0, 1.0, 0.0), -M_PI/2.0)))
            .component(new ObjDrawable("bunny.obj"))
        )
        // camera positioned behind and slightly above the bunny
        .child(VObjectConfig()
            .component(main_camera)
        );
}

VObjectConfig Planet() {
    return VObjectConfig()
        .child(VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, -10.0f, 0.0f))
            )
            .component(new ObjDrawable("sphere.obj"))
        );
}

namespace game::scenes {
    void MainScene::hierarchy(SceneRoot& root) {
        const float default_planet_radius = 10.0f; // Very precise estimate
        const float planet_radius = 170.0f;
        const float planet_scale = planet_radius / default_planet_radius;

        Camera* first_person_camera = new Camera();
        Camera* third_person_camera = new Camera();
        Camera::set_main(third_person_camera);
        
        InputController* input = EngineController::get_input();
        input->subscribe_press_button(GLFW_KEY_C, [third_person_camera, first_person_camera]() {
            if (Camera::get_main() == third_person_camera) {
                Camera::set_main(first_person_camera);
            } else {
                Camera::set_main(third_person_camera);
            }
        });
    
        VObjectConfig third_person_camera_config;
        third_person_camera_config.component(third_person_camera);
        root.vobject(third_person_camera_config);

        PlayerController* player_controller = nullptr;
        VObjectConfig player_config = Player(first_person_camera, planet_radius, &player_controller);
            player_config.transform(TransformBuilder().position(Vec3(10.0f*planet_radius, planet_radius, planet_radius)));

        VObjectConfig camera_controller_config;
        CameraController* camera_controller = new CameraController(player_controller, third_person_camera);
            camera_controller_config.component(camera_controller);
            camera_controller_config.component(new Stars(player_controller, 20000));

        VObjectConfig particles_config;
            particles_config.component(new Particles(100));

        VObjectConfig main_planet_config = Planet();
            main_planet_config.transform(TransformBuilder().scale(planet_scale));

        VObjectConfig satellitesRoot;
            std::mt19937 rng(123456);

            const int N_rings = 5;
            const int per_ring = 4; // total = N_rings * per_ring
            const float minR = planet_radius * 10.0f;
            const float maxR = planet_radius * 100.0f;
            std::vector<float> rings;
            rings.reserve(N_rings);
            float ring_step = (maxR - minR) / static_cast<float>(std::max(1, N_rings - 1));
            for (int r = 0; r < N_rings; ++r) rings.push_back(minR + r * ring_step);

            std::uniform_real_distribution<float> angleJitter(-0.08f, 0.08f);
            std::uniform_real_distribution<float> radiusJitter(-ring_step * 0.15f, ring_step * 0.15f);
            std::uniform_real_distribution<float> yDist(-planet_radius * 0.5f, planet_radius * 0.5f);
            std::uniform_real_distribution<float> scaleDist(0.35f, 0.75f); // relative to planet_scale
            std::uniform_real_distribution<float> speedDist(0.003f, 0.005f);
            std::uniform_real_distribution<float> tiltDist(-0.04f, 0.04f); // small common tilt (~±2.3°)
            std::uniform_real_distribution<float> phaseDist(0.0f, 1.0f); // random starting phase [0,1)

            // single small tilt for all orbits (keeps orbits nearly coplanar)
            float common_tilt = tiltDist(rng);
            float sin_t = std::sin(common_tilt);
            float cos_t = std::cos(common_tilt);
            engine::Vec3 common_orbit_normal(sin_t, cos_t, 0.0f);

            for (int r = 0; r < N_rings; ++r) {
                for (int k = 0; k < per_ring; ++k) {
                    float radius = rings[r] + radiusJitter(rng);
                    float s = scaleDist(rng) * planet_scale;
                    float phase = phaseDist(rng);
                    float speed = speedDist(rng);

                    satellitesRoot.child(Planet()
                        .transform(TransformBuilder()
                            .scale(s))
                        .component(new Trajectory(
                            std::make_unique<PhaseCircularCurve>(engine::Vec3(0.0f), common_orbit_normal, radius, phase),
                            speed
                        ))
                    );
                }
            }

        root.vobject(player_config);
        root.vobject(camera_controller_config);
        root.vobject(main_planet_config);
        root.vobject(satellitesRoot);

    }
}
