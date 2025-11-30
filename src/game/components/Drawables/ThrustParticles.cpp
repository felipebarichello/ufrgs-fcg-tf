#include "ThrustParticles.hpp"

using namespace engine;

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
        float delta_time = engine::EngineController::get_delta_time();
        SpaceshipController* ship_controller = this->ship_controller;
        bool thrusting = ship_controller->get_command().thrusting && ship_controller->fuel > 0.0f;
        engine::Vec3 ship_velocity = ship_controller->get_kinematic_body()->get_velocity();

        // Use the mandatory thruster transform (provided in constructor)
        Transform* thruster_transf = this->thruster_transform();
        engine::Vec3 emit_pos = thruster_transf->get_world_position();
        auto q = thruster_transf->get_world_quaternion();
        engine::Vec3 emit_forward = normalize(q.rotate(engine::Vec3(0.0f, 0.0f, -1.0f)));
        engine::Vec3 emit_right = normalize(q.rotate(engine::Vec3(1.0f, 0.0f, 0.0f)));
        engine::Vec3 emit_up = normalize(q.rotate(engine::Vec3(0.0f, 1.0f, 0.0f)));

        // Spawn particles based on particles_per_second while thrusting.
        if (thrusting) {
            float spawn_f = this->particles_per_second * delta_time + this->spawn_accumulator;
            int to_spawn = static_cast<int>(spawn_f);
            this->spawn_accumulator = spawn_f - static_cast<float>(to_spawn);

            for (int s = 0; s < to_spawn; ++s) {
                // random lateral offset
                float radial_distance = static_cast<float>(rand()) / RAND_MAX * this->thruster_radius;
                float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * 3.1415926f;
                float rx = std::cos(angle) * radial_distance;
                float ry = std::sin(angle) * radial_distance;
                float depth = static_cast<float>(rand()) / RAND_MAX * this->spawn_depth;

                // Compute world-space base position using local offset
                Vec3 base_pos = emit_pos + q.rotate(this->thruster_offset);
                Vec3 forward_world = q.rotate(this->thruster_normal);

                Vec3 jitter = normalize(Vec3(
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * this->spread_jitter,
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * this->spread_jitter,
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * this->spread_jitter
                ));

                Vec3 dir = normalize(-forward_world + jitter);
                float speed = this->min_particle_speed + (static_cast<float>(rand()) / RAND_MAX) * (this->max_particle_speed - this->min_particle_speed);

                float decay = min_particle_decay_rate + (static_cast<float>(rand()) / RAND_MAX) * (max_particle_decay_rate - min_particle_decay_rate);
                float size = max_particle_size * (static_cast<float>(rand()) / RAND_MAX);

                Particle newp = Particle {
                    .position = base_pos + forward_world * (-depth) + emit_right * rx + emit_up * ry,
                    .velocity = dir * speed + ship_velocity,
                    .decay_time = decay,
                    .size = size,
                    .age = 0.0f,
                };

                // try to reuse an expired particle slot first
                bool placed = false;
                for (auto &pp : particles) {
                    if (pp.age >= pp.decay_time) {
                        pp = newp;
                        placed = true;
                        break;
                    }
                }
                if (!placed) particles.push_back(newp);
            }
        }

        for (auto& particle : particles) {
            // Update age and position
            particle.age += delta_time;
            particle.position += particle.velocity * delta_time;

            bool expired = particle.age >= particle.decay_time;

            // if expired or moved beyond forward limit, respawn behind thruster
            engine::Vec3 to_particle = particle.position - emit_pos;
            float forward_dot = to_particle.x * emit_forward.x + to_particle.y * emit_forward.y + to_particle.z * emit_forward.z;
            const float emission_depth = this->spawn_depth;

            if (expired || forward_dot >= emission_depth) {
                // Only respawn particles while thrusting; when not thrusting let them die out
                if (!thrusting) {
                    continue;
                }
                
                float radial_distance = (static_cast<float>(rand()) / RAND_MAX) * this->thruster_radius;
                float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * 3.1415926f;
                float rx = std::cos(angle) * radial_distance;
                float ry = std::sin(angle) * radial_distance;

                Vec3 base_pos = emit_pos + q.rotate(this->thruster_offset);
                Vec3 forward_world = q.rotate(this->thruster_normal);

                Vec3 spawn_position = base_pos + forward_world * (- (static_cast<float>(rand()) / RAND_MAX) * emission_depth)
                    + emit_right * rx + emit_up * ry;

                Vec3 jitter = normalize(Vec3(
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * (this->spread_jitter * 0.8333333f),
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * (this->spread_jitter * 0.8333333f),
                    (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * (this->spread_jitter * 0.8333333f)
                ));
                Vec3 dir = normalize(-forward_world + jitter);
                float speed = this->min_particle_speed + (static_cast<float>(rand()) / RAND_MAX) * (this->max_particle_speed - this->min_particle_speed); // arbitrary speed range

                particle.decay_time = min_particle_decay_rate + (static_cast<float>(rand()) / RAND_MAX) * (max_particle_decay_rate - min_particle_decay_rate);
                if (particle.decay_time <= 0.0f) particle.decay_time = min_particle_decay_rate;
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
        }
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
            float alpha = 1.0f - t * 0.9f; // fade out towards end of life
            engine::Vec4 color4(col.x, col.y, col.z, alpha);
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
