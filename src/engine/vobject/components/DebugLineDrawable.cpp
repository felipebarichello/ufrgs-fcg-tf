#include "DebugLineDrawable.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <engine/EngineController.hpp>

using namespace engine;

DebugLineDrawable::DebugLineDrawable(std::function<Vec3()> p1_getter, std::function<Vec3()> p2_getter, engine::Vec4 color)
    : p1_getter(std::move(p1_getter)), p2_getter(std::move(p2_getter)), color(color)
{
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    // Reserve space for two vertices: vec4 pos + vec4 normal + vec2 tex = 10 floats
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10 * 2, NULL, GL_DYNAMIC_DRAW);

    // position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(0));
    // normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 4));
    // texcoord (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(sizeof(float) * 8));

    glBindVertexArray(0);
}

DebugLineDrawable::~DebugLineDrawable() {
    if (this->vbo) glDeleteBuffers(1, &this->vbo);
    if (this->vao) glDeleteVertexArrays(1, &this->vao);
}

void DebugLineDrawable::draw() {
    if (!this->p1_getter || !this->p2_getter) return;

    Vec3 p1 = this->p1_getter();
    Vec3 p2 = this->p2_getter();

    float data[20];
    data[0] = p1.x; data[1] = p1.y; data[2] = p1.z; data[3] = 1.0f;
    data[4] = 0.0f; data[5] = 0.0f; data[6] = 0.0f; data[7] = 0.0f;
    data[8] = 0.0f; data[9] = 0.0f;
    data[10] = p2.x; data[11] = p2.y; data[12] = p2.z; data[13] = 1.0f;
    data[14] = 0.0f; data[15] = 0.0f; data[16] = 0.0f; data[17] = 0.0f;
    data[18] = 0.0f; data[19] = 0.0f;

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);

    GLuint prog = EngineController::get_program_id(EngineController::ShaderType::Line);
    glUseProgram(prog);
    GLint model_loc = glGetUniformLocation(prog, "model");
    if (model_loc != -1) {
        glm::mat4 identity(1.0f);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(identity));
    }
    GLint color_loc = glGetUniformLocation(prog, "absolute_color");
    if (color_loc != -1) {
        glUniform4f(color_loc, this->color.x, this->color.y, this->color.z, this->color.w);
    }

    // Optionally disable depth test so the line is visible over geometry
    GLboolean depth_enabled = glIsEnabled(GL_DEPTH_TEST);
    if (depth_enabled) glDisable(GL_DEPTH_TEST);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 2);

    if (depth_enabled) glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
}
