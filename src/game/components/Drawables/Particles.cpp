#include "Particles.hpp"

Particles::Particles(int max_particles, float* speed) : speed(speed) {
    this->max_particles = max_particles;
    this->particles.reserve(max_particles);
    this->speed = speed;
    
    // Initialize particles with random positions and parameters
    for (int i = 0; i < max_particles; ++i) {
        Particle p;
        p.radial_distance = static_cast<float>(rand()) / RAND_MAX * 10.0f; // Random radial distance
        p.theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f; // Random angle
        p.position = engine::Vec3(
            p.radial_distance * cos(p.theta),
            p.radial_distance * sin(p.theta),
            0.0f
        );
        particles.push_back(p);
    }

    engine::VaoBuilder builder;
    float model_coefficients[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    builder.add_vbo(0, 4, sizeof(model_coefficients), model_coefficients, GL_STATIC_DRAW);
    // builder.build returns an engine::Vao by value; allocate a copy on the heap to assign to the pointer member
    this->vao_ptr = new engine::Vao(builder.build(GL_POINTS, 1, GL_UNSIGNED_INT));
}
Particles::~Particles() {
    // Cleanup allocated VAO
    if (this->vao_ptr) {
        delete this->vao_ptr;
        this->vao_ptr = nullptr;
    }
}

void Particles::update() {
    float delta_time = engine::EngineController::get_delta_time();
    for (auto& particle : particles) {
        particle.position.z += *this->speed * delta_time;
        if (particle.position.z > this->max_distance) {
            particle.position.z = 0.0f;
        }
    }
}

void Particles::draw() {

    GLuint program_id;

    this->update();

    if (this->use_phong_shading)
        program_id = engine::EngineController::get_phong_program_id();
    else
        program_id = engine::EngineController::get_gouraud_program_id();

    glUseProgram(program_id);

    GLint Kd = glGetUniformLocation(program_id, "diffuse_reflectance");
    GLint Ks = glGetUniformLocation(program_id, "specular_reflectance");
    GLint Ka = glGetUniformLocation(program_id, "enviornment_reflectance");
    GLint q  = glGetUniformLocation(program_id, "specular_exponent");
    
    glUniform3fv(Kd, 1, &this->diffuse_reflectance[0]);
    glUniform3fv(Ks, 1, &this->specular_reflectance[0]);
    glUniform3fv(Ka, 1, &this->environment_reflectance[0]);
    glUniform1f(q, this->specular_exponent);

    glm::mat4 vobject_matrix = this->get_vobject()->transform().get_model_matrix();

    for (const auto& particle : particles) {
        glm::mat4 model_matrix = glm::translate(vobject_matrix, glm::vec3(particle.position));
        GLsizei model_uniform = glGetUniformLocation(program_id, "model");
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model_matrix));
        if (this->vao_ptr) {
            this->vao_ptr->draw();
        }
    }
}