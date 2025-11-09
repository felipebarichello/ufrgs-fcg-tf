#include "Stars.hpp"
#include <cmath>
#include <random>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <engine/math/matrices.hpp>

Stars::Stars(int max_stars) {
    this->max_stars = max_stars;
    this->stars.reserve(max_stars);

    // Random generator for positions on the sphere and for size/alpha
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist_phi(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> dist_cos_theta(-1.0f, 1.0f);
    std::uniform_real_distribution<float> dist_size(0.5f, 3.0f);
    std::uniform_real_distribution<float> dist_alpha(0.25f, 1.0f);

    for (int i = 0; i < max_stars; ++i) {
        Star s;
        s.phi = dist_phi(rng);
        float u = dist_cos_theta(rng);
        s.theta = std::acos(u);
        float sin_t = std::sin(s.theta);
        s.position = engine::Vec3(
            this->distance * sin_t * std::cos(s.phi),
            this->distance * sin_t * std::sin(s.phi),
            this->distance * std::cos(s.theta)
        );
        s.alpha = dist_alpha(rng);
        s.size = dist_size(rng);
        stars.push_back(s);
    }

    std::vector<float> model_coeffs;
    std::vector<float> sizes;
    std::vector<float> alphas;
    model_coeffs.reserve(max_stars * 4);
    sizes.reserve(max_stars);
    alphas.reserve(max_stars);

    for (const auto &s : stars) {
        model_coeffs.push_back(s.position.x);
        model_coeffs.push_back(s.position.y);
        model_coeffs.push_back(s.position.z);
        model_coeffs.push_back(1.0f); // w = 1.0 for model_coefficients

        sizes.push_back(s.size);
        alphas.push_back(s.alpha);
    }

    engine::VaoBuilder builder;
    // location 0: vec4 model_coefficients
    builder.add_vbo(0, 4, model_coeffs.size() * sizeof(float), model_coeffs.data(), GL_STATIC_DRAW);
    // location 1: float point_size
    builder.add_vbo(1, 1, sizes.size() * sizeof(float), sizes.data(), GL_STATIC_DRAW);
    // location 2: float point_alpha
    builder.add_vbo(2, 1, alphas.size() * sizeof(float), alphas.data(), GL_STATIC_DRAW);

    this->vao_ptr = new engine::Vao(builder.build(GL_POINTS, static_cast<GLsizei>(max_stars), 0));
}

Stars::~Stars() {
    if (this->vao_ptr) {
        delete this->vao_ptr;
        this->vao_ptr = nullptr;
    }
}

void Stars::update() {
    engine::Camera* cam = engine::Camera::get_main();
    if (cam && this->get_vobject()) {
        auto player_pos = cam->get_vobject()->transform().get_position();
        this->get_vobject()->transform().position() = player_pos;
    }
}

void Stars::draw() {
    GLuint program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Star);
    if (program_id == 0) return; // shaders not ready

    glUseProgram(program_id);
    glEnable(GL_PROGRAM_POINT_SIZE);

    GLint model_uniform = glGetUniformLocation(program_id, "model");
    GLint view_uniform = glGetUniformLocation(program_id, "view");
    GLint proj_uniform = glGetUniformLocation(program_id, "projection");
    GLint color_uniform = glGetUniformLocation(program_id, "absolute_color");

    engine::Camera* cam = engine::Camera::get_main();

    // Compute transform so the starfield stays centered on the player
    glm::mat4 vobject_matrix(1.0f);
    if (cam && cam->get_vobject()) {
        auto player_vo = cam->get_vobject();
        glm::mat4 player_model = player_vo->transform().get_model_matrix();
        glm::vec3 player_position = glm::vec3(player_model[3]);
        vobject_matrix = vobject_matrix * engine::h_translate_matrix(player_position.x, player_position.y, player_position.z);
    }

    glm::mat4 view = engine::Camera::get_main()->get_view_matrix();
    glm::mat4 proj = engine::Camera::get_main()->get_perspective_matrix();

    if (model_uniform >= 0) glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(vobject_matrix));
    if (view_uniform >= 0) glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
    if (proj_uniform >= 0) glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

    if (color_uniform >= 0) {
        glm::vec4 c = glm::vec4(this->color, 1.0f);
        glUniform4fv(color_uniform, 1, glm::value_ptr(c));
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    if (this->vao_ptr) this->vao_ptr->draw();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}