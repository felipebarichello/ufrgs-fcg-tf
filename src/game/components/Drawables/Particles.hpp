#pragma once

#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Drawable.hpp>
#include <engine/vobject/components/Camera.hpp>
#include <engine/EngineController.hpp>
#include <engine/vao.hpp>
#include <engine/math/matrices.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <game/components/player/CameraController.hpp>

class Particles : public engine::Drawable {
public:
    Particles(int max_particles);
    ~Particles();

    void update();
    void draw() override;

private:
    struct Particle {
        engine::Vec3 direction;
        engine::Vec3 position;
        engine::Vec4 color; // rgba
        float speed_mul = 1.0f; // per-particle speed multiplier to avoid synchronization
        float age = 0.0f;       // current life progress (seconds)
        float lifetime = 1.0f;  // life duration (seconds)
        float size = 1.0f;
    };

    std::vector<Particle> particles;
    int max_particles;
    float max_radial_distance = 20.0f;
    float max_distance = 20.0f;
    float particle_min_lifetime = 1.0f; // seconds
    float particle_max_lifetime = 4.0f; // seconds
    bool use_phong_shading = false;
    engine::Vec3 color = {1.0f, 1.0f, 1.0f};
    bool first_time = true;
    float max_particle_size = 3.0f;
};  