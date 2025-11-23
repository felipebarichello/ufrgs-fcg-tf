#include "SpaceParticles.hpp"

using namespace engine;

SpaceParticles::SpaceParticles(int max_particles) {
    this->max_particles = max_particles;
    this->particles.reserve(max_particles);
    // Setup a minimal VAO used to draw a single point; actual particle
    // positions are provided via a uniform per-draw call.
    engine::VaoBuilder builder;
    float model_coefficients[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    builder.add_vbo(0, 4, sizeof(model_coefficients), model_coefficients, GL_STATIC_DRAW);
    this->vao_ptr = new engine::Vao(builder.build(GL_POINTS, 1, GL_UNSIGNED_INT));

    // Initialize particles with sensible defaults. If a main camera exists and
    // has a vobject, spawn particles relative to the camera; otherwise spawn
    // around the origin. Particles are placed in front of the camera (along
    // its forward vector) and will move toward the camera.
    engine::Camera* cam = engine::Camera::get_main();
    engine::Vec3 cam_pos(0.0f);
    engine::Quaternion cam_q = engine::Quaternion::identity();
    if (cam && cam->get_vobject() != nullptr) {
        cam_pos = cam->get_vobject()->transform().get_local_position();
        cam_q = cam->get_vobject()->transform().get_quaternion();
    }

    engine::Vec3 forward = cam_q.rotate(engine::Vec3(0.0f, 0.0f, -1.0f));
    engine::Vec3 right = cam_q.rotate(engine::Vec3(1.0f, 0.0f, 0.0f));
    engine::Vec3 up = cam_q.rotate(engine::Vec3(0.0f, 1.0f, 0.0f));

    for (int i = 0; i < max_particles; ++i) {
        Particle p;
        float radial_distance = static_cast<float>(rand()) / RAND_MAX * this->max_radial_distance;
        float rx = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * radial_distance;
        float ry = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * radial_distance;
        float depth = static_cast<float>(rand()) / RAND_MAX * this->max_distance;

        // Position = camera_pos + forward*depth + lateral jitter
        p.position = cam_pos + forward * depth + right * rx + up * ry;
        // Move in a random direction away from the camera
        p.direction = engine::normalize(Vec3(
            (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f),
            (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f),
            (static_cast<float>(rand()) / RAND_MAX * 0.5f + 0.5f) // bias forward
        ));
        p.color = engine::Vec4(1.0f, 1.0f, 1.0f, 0.6f);
        // randomize per-particle speed multiplier and lifetime to avoid sync
        p.speed_mul = 0.6f + (static_cast<float>(rand()) / RAND_MAX) * 1.4f; // [0.6,2.0]
        p.lifetime = particle_min_lifetime + (static_cast<float>(rand()) / RAND_MAX) * (particle_max_lifetime - particle_min_lifetime);
        // randomize initial age so not all particles die together
        p.age = static_cast<float>(rand()) / RAND_MAX * p.lifetime;
        p.size = max_particle_size * (static_cast<float>(rand()) / RAND_MAX);
        particles.push_back(p);
    }
}

SpaceParticles::~SpaceParticles() {
    if (this->vao_ptr) {
        delete this->vao_ptr;
        this->vao_ptr = nullptr;
    }
}

void SpaceParticles::update() {
    float delta_time = engine::EngineController::get_delta_time();


    // get the camera forward; guard against null camera or vobject
    auto camera = engine::Camera::get_main();
    engine::Vec3 camera_forward = engine::Vec3(0.0f, 0.0f, -1.0f);
    engine::Vec3 camera_right = engine::Vec3(1.0f, 0.0f, 0.0f);
    engine::Vec3 camera_up = engine::Vec3(0.0f, 1.0f, 0.0f);
    engine::Vec3 camera_postion = engine::Vec3(0.0f);
    if (camera && camera->get_vobject() != nullptr) {
        auto q = camera->get_vobject()->transform().get_quaternion();
        camera_forward = normalize(q.rotate(engine::Vec3(0.0f, 0.0f, -1.0f)));
        camera_right = normalize(q.rotate(engine::Vec3(1.0f, 0.0f, 0.0f)));
        camera_up = normalize(q.rotate(engine::Vec3(0.0f, 1.0f, 0.0f)));
        camera_postion = camera->get_vobject()->transform().get_local_position();
    }

    for (auto& particle : particles) {

        particle.age += delta_time * particle.speed_mul;
        
        particle.color.a += delta_time / particle.lifetime; // fade in over lifetime
        bool expired = particle.age >= particle.lifetime;
        engine::Vec3 to_particle = particle.position - camera_postion;
        float forward_dot = to_particle.x * camera_forward.x + to_particle.y * camera_forward.y + to_particle.z * camera_forward.z;
        particle.position += particle.direction * delta_time * particle.speed_mul;
        if (expired || forward_dot <= 0.0f) {
            first_time = false;

            //particle.direction = -camera_forward;
            particle.position = camera_postion + camera_forward * this->max_distance;

            particle.position += camera_right * (static_cast<float>(rand()) / RAND_MAX - 0.5f) * this->max_radial_distance;
            particle.position += camera_up * (static_cast<float>(rand()) / RAND_MAX - 0.5f) * this->max_radial_distance;

            particle.lifetime = particle_min_lifetime + (static_cast<float>(rand()) / RAND_MAX) * (particle_max_lifetime - particle_min_lifetime);
            particle.age = 0.0f;
            particle.color.a = 0.5f;
        }
    }
}

void SpaceParticles::draw() {

    this->update();

    GLuint program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Particle);
    if (program_id == 0) {
        // fallback to planet/gouraud if particle program not available
        program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Planet);
    }

    glUseProgram(program_id);
    GLint color_uniform = glGetUniformLocation(program_id, "absolute_color");
    GLint model_uniform = glGetUniformLocation(program_id, "model");
    GLint position_uniform = glGetUniformLocation(program_id, "position");

    engine::Mat4 model_matrix = engine::Mat4(1.0f);

    // Enable blending so particle alpha is applied, and disable depth writes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    for (const auto& particle : particles) {
        glPointSize(particle.size);
        if (position_uniform != -1) {
            // shader declares a vec4 position; upload a vec4
            engine::Vec4 pos4(particle.position, 1.0f);
            glUniform4fv(position_uniform, 1, glm::value_ptr(pos4));
        }
        if (model_uniform != -1) glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
        if (color_uniform != -1) glUniform4fv(color_uniform, 1, glm::value_ptr(particle.color));
        if (this->vao_ptr) {
            this->vao_ptr->draw();
        }
    }

    // Restore depth write and blending state
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);

}