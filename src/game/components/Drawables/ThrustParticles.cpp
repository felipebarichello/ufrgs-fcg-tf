#include "ThrustParticles.hpp"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

using namespace engine;

// Sample a direction uniformly inside a cone around `axis`.
// `cone_half_angle` is the cone half-angle in radians (0 = no spread).
static engine::Vec3 sample_direction_in_cone(const engine::Vec3 &axis, float cone_half_angle) {
    float u = static_cast<float>(rand()) / RAND_MAX; // [0,1)
    float cos_max = std::cos(cone_half_angle);
    float cos_theta = 1.0f - u * (1.0f - cos_max); // uniform on [cos_max, 1]
    float sin_theta = std::sqrt(std::max(0.0f, 1.0f - cos_theta * cos_theta));
    float phi = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * 3.14159265358979323846f;

    float x = std::cos(phi) * sin_theta;
    float y = std::sin(phi) * sin_theta;
    float z = cos_theta;

    engine::Vec3 axis_n = normalize(axis);
    // pick a vector not parallel to axis to build an orthonormal basis
    engine::Vec3 tmp = (std::fabs(axis_n.z) < 0.999f) ? engine::Vec3(0.0f, 0.0f, 1.0f) : engine::Vec3(0.0f, 1.0f, 0.0f);
    engine::Vec3 right = normalize(glm::cross(axis_n, tmp));
    engine::Vec3 up = glm::cross(right, axis_n);

    engine::Vec3 dir = right * x + up * y + axis_n * z;
    return normalize(dir);
}

namespace game::components {
    ThrustParticles::ThrustParticles(SpaceshipController* ship_controller) {
        this->ship_controller = ship_controller;

        // Setup a minimal VAO used to draw a single point; actual particle
        // positions are provided via a uniform per-draw call.
        engine::VaoBuilder builder;
        float model_coefficients[] = {
            0.0f, 0.0f, 0.0f, 1.0f,
        };

        builder.add_vbo(0, 4, sizeof(model_coefficients), model_coefficients, GL_STATIC_DRAW);
        this->vao_ptr = new Vao(builder.build(GL_POINTS, 1, GL_UNSIGNED_INT));

        // Start with an empty particle pool. Particles will be initialized
        // only once a thruster VObject is provided via `set_thruster()`.
        particles.clear();
        particles.reserve(64);
    }

    ThrustParticles::~ThrustParticles() {
        if (this->vao_ptr) {
            delete this->vao_ptr;
            this->vao_ptr = nullptr;
        }
    }

    void ThrustParticles::update() {
        float dt = engine::EngineController::get_delta_time();
        SpaceshipController* ship_controller = this->ship_controller;
        bool thrusting = ship_controller->get_command().thrusting && ship_controller->fuel > 0.0f;
        engine::Vec3 ship_velocity = ship_controller->get_kinematic_body()->get_velocity();

        // Use the thruster transform (provided in constructor)
        Transform* thruster_transf = this->thruster_transform();
        engine::Vec3 emit_pos = thruster_transf->get_world_position();
        Quaternion q = thruster_transf->get_world_quaternion();
        engine::Vec3 emit_right = normalize(q.rotate(engine::Vec3(1.0f, 0.0f, 0.0f)));
        engine::Vec3 emit_up = normalize(q.rotate(engine::Vec3(0.0f, 1.0f, 0.0f)));
        Vec3 base_pos = emit_pos + q.rotate(this->thruster_offset);
        Vec3 normal_dir = q.rotate(this->thruster_normal);

        int new_particles_needed;

        if (thrusting) {
            float frac_particles_needed = this->particles_per_second * dt + this->spawn_accumulator;
            new_particles_needed = static_cast<int>(frac_particles_needed);
            this->spawn_accumulator = frac_particles_needed - static_cast<float>(new_particles_needed);
        } else {
            new_particles_needed = 0;
            this->spawn_accumulator = 0.0f;
        }

        size_t cur_particle_count = particles.size();
        for (size_t i = 0; i < cur_particle_count; i++) {
            Particle& particle = particles[i];
            particle.age += dt;
            bool expired = particle.age >= particle.decay_time;

            if (expired) {
                if (new_particles_needed > 0) {
                    this->init_particle(
                        particle,
                        ship_velocity,
                        emit_right,
                        emit_up,
                        normal_dir,
                        base_pos
                    );
                    new_particles_needed--;
                }
                
                continue;
            }

            particle.position += particle.velocity * dt;
        }

        if (new_particles_needed > 0) {
            // Create new particles
            for (int i = 0; i < new_particles_needed; i++) {
                Particle new_particle;
                this->init_particle(
                    new_particle,
                    ship_velocity,
                    emit_right,
                    emit_up,
                    normal_dir,
                    base_pos
                );
                particles.push_back(new_particle);
            }
        }
    }

    void ThrustParticles::init_particle(
        Particle& particle,
        Vec3& ship_velocity,
        Vec3& emit_right,
        Vec3& emit_up,
        Vec3& normal_dir,
        Vec3& base_pos
    ) {
        float radial_distance = (static_cast<float>(rand()) / RAND_MAX) * this->thruster_radius;
        float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * 3.1415926f;
        float rx = std::cos(angle) * radial_distance;
        float ry = std::sin(angle) * radial_distance;

        Vec3 spawn_position = base_pos + normal_dir * (- (static_cast<float>(rand()) / RAND_MAX) * this->spawn_depth)
            + emit_right * rx + emit_up * ry;

        // spawn particles moving roughly opposite the thruster normal with spread
        Vec3 dir = sample_direction_in_cone(normal_dir, this->spread_cone);
        float speed = this->min_particle_speed + (static_cast<float>(rand()) / RAND_MAX) * (this->max_particle_speed - this->min_particle_speed); // arbitrary speed range

        particle.decay_time = min_particle_decay_time + (static_cast<float>(rand()) / RAND_MAX) * (max_particle_decay_time - min_particle_decay_time);
        if (particle.decay_time <= 0.0f) particle.decay_time = min_particle_decay_time;
        particle.age = 0.0f;
        particle.size = max_particle_size * (static_cast<float>(rand()) / RAND_MAX);

        particle = Particle {
            .position = spawn_position,
            .velocity = dir * speed + ship_velocity,
            .decay_time = particle.decay_time,
            .size = particle.size,
            .age = 0.0f,
        };
    }

    void ThrustParticles::draw() {

        // thruster_transform is mandatory and provided in constructor; update and draw
        this->update();

        GLuint program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Particle);
        if (program_id == 0) {
            // fallback to planet/gouraud if particle program not available
            program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Planet);
        }

        glUseProgram(program_id);
        // get uniform for point size so shader can set gl_PointSize
        GLint point_size_uniform = glGetUniformLocation(program_id, "point_size");
        GLint color_uniform = glGetUniformLocation(program_id, "absolute_color");
        GLint model_uniform = glGetUniformLocation(program_id, "model");
        GLint position_uniform = glGetUniformLocation(program_id, "position");

        engine::Mat4 model_matrix = engine::Mat4(1.0f);

        // Enable additive blending for fire-like particles and allow shader point size
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_PROGRAM_POINT_SIZE);

        for (const auto& particle : particles) {
            // supply size to shader via uniform when available
            // scale up particle size for better visibility
            float display_size = particle.size * 8.0f;
            if (point_size_uniform != -1) {
                glUniform1f(point_size_uniform, display_size);
            } else {
                glPointSize(display_size);
            }
            if (position_uniform != -1) {
                // shader declares a vec4 position; upload a vec4
                engine::Vec4 pos4(particle.position, 1.0f);
                glUniform4fv(position_uniform, 1, glm::value_ptr(pos4));
            }
            if (model_uniform != -1) glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));

            // compute particle color by interpolating initial->final by age/decay
            float t = 0.0f;
            if (particle.decay_time > 0.0f) t = particle.age / particle.decay_time;
            // speed up transition towards final color
            t *= this->color_transition_speed;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            engine::Vec3 col = this->initial_color * (1.0f - t) + this->final_color * t;
            float alpha_factor = 1.0f - std::pow(t, 4.0f); // fade out towards end of life
            engine::Vec4 color4(col.x, col.y, col.z, alpha_factor * this->initial_alpha);
            if (color_uniform != -1) glUniform4fv(color_uniform, 1, glm::value_ptr(color4));

            if (this->vao_ptr) {
                this->vao_ptr->draw();
            }
        }

        // Restore blend and point size state
        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
} // namespace game::components
