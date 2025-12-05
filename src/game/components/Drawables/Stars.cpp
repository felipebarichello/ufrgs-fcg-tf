#include "Stars.hpp"
#include <cmath>
#include <random>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <engine/math/linalg.hpp>

Stars::Stars(int max_stars) {
    this->max_stars = max_stars;
    this->stars.reserve(max_stars);

    // Random star generation
    std::mt19937 rng(12345);

    std::uniform_real_distribution<float>dist_phi(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float>dist_cos_theta(-1.0f, 1.0f);

    std::uniform_real_distribution<float>dist_size(this->min_size, this->max_size);
    std::uniform_real_distribution<float>dist_alpha(this->min_alpha, this->max_alpha);

    // Create stars
    for (int i = 0; i < max_stars; ++i) {
        Star star;

        star.phi = dist_phi(rng);
        star.theta = acos(dist_cos_theta(rng));

        float x = this->distance * sin(star.theta) * cos(star.phi);
        float y = this->distance * sin(star.theta) * sin(star.phi);
        float z = this->distance * cos(star.theta);

        star.position = engine::Vec3(x, y, z);
        star.alpha = dist_alpha(rng);
        star.size = dist_size(rng);

        stars.push_back(star);
    }

    std::vector<float>model_coeffs;
    std::vector<float>sizes;
    std::vector<float>alphas;

    // Populate buffers
    for (const auto &s : stars) {
        model_coeffs.push_back(s.position.x);
        model_coeffs.push_back(s.position.y);
        model_coeffs.push_back(s.position.z);
        model_coeffs.push_back(1.0f); 

        sizes.push_back(s.size);
        alphas.push_back(s.alpha);
    }

    // Create VAO
    engine::VaoBuilder builder;
    builder.add_vbo(0, 4, model_coeffs.size() * sizeof(float), model_coeffs.data(), GL_STATIC_DRAW);
    builder.add_vbo(1, 1, sizes.size() * sizeof(float), sizes.data(), GL_STATIC_DRAW);
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
    // Keep the stars centered on the camera/player ship
    engine::Camera* cam = engine::Camera::get_main();
    if (cam && this->get_vobject()) {
        auto player_pos = cam->get_vobject()->transform().get_world_position();
        this->get_vobject()->transform().set_world_position(player_pos);
    }
}

void Stars::draw() {
    GLuint program_id = engine::EngineController::get_program_id(engine::EngineController::ShaderType::Star);

    glUseProgram(program_id);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Set uniforms
    GLint model_uniform = glGetUniformLocation(program_id, "model");
    GLint view_uniform = glGetUniformLocation(program_id, "view");
    GLint proj_uniform = glGetUniformLocation(program_id, "projection");
    GLint color_uniform = glGetUniformLocation(program_id, "absolute_color");

    // Get player position
    auto player_vobject = engine::Camera::get_main()->get_vobject();
    engine::Mat4 player_model = player_vobject->transform().get_model_matrix();
    engine::Vec3 player_position = engine::Vec3(player_model[3]);
    engine::Mat4 vobject_matrix = engine::h_translate_matrix(player_position.x, player_position.y, player_position.z);
    engine::Mat4 view = engine::Camera::get_main()->get_view_matrix();
    engine::Mat4 proj = engine::Camera::get_main()->get_perspective_matrix();

    // Upload matrices
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(vobject_matrix));
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, glm::value_ptr(proj));

    // Upload color
    engine::Vec4 c = engine::Vec4(this->color, 1.0f);
    glUniform4fv(color_uniform, 1, glm::value_ptr(c));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    this->vao_ptr->draw();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}