#include "vao.hpp"

namespace engine {

    // Position
    Position::Position() {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        w = 0.0;
    }

    Position::Position(GLfloat x, GLfloat y, GLfloat z) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1.0;
    }

    Position::Position(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    // Color
    Color::Color() {
        r = 0.0;
        g = 0.0;
        b = 0.0;
        a = 0.0;
    }

    Color::Color(GLfloat r, GLfloat g, GLfloat b) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 1.0;
    }

    Color::Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    // Vertex
    Vertex::Vertex(Position position, Color color) {
        this->position = position;
        this->color = color;
    }

    // Vao
    Vao::Vao(GLuint vao_id, GLenum topology_mode, GLsizei topology_size, GLenum topology_type) {
        this->vao_id = vao_id;
        this->topology_mode = topology_mode;
        this->topology_size = topology_size;
        this->topology_type = topology_type;
    }

    void Vao::draw() const {
        this->bind();
        // If no element array buffer (topology_type == 0) was provided, draw arrays
        if (this->topology_type == 0) {
            glDrawArrays(this->topology_mode, 0, this->topology_size);
        } else {
            glDrawElements(this->topology_mode, this->topology_size, this->topology_type, 0);
        }
        this->unbind();
    }

    void Vao::bind() const {
        glBindVertexArray(this->vao_id);
    }

    void Vao::unbind() const {
        glBindVertexArray(0);
    }

    // Implementation of id() accessor
    GLuint Vao::id() const {
        return this->vao_id;
    }
    

    // VaoBuilder
    VaoBuilder::VaoBuilder() {
        glGenVertexArrays(1, &this->vao_id);
        glBindVertexArray(this->vao_id);
    }

    VaoBuilder::~VaoBuilder() {
        glBindVertexArray(0);
    }

    VaoBuilder& VaoBuilder::add_vbo(GLuint location, GLint number_of_dimensions, size_t buffer_size, void* data, GLenum usage_hint) {
        this->add_buffer(GL_ARRAY_BUFFER, buffer_size, data, usage_hint);
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return *this;
    }

    VaoBuilder& VaoBuilder::add_ebo(size_t buffer_size, void* data, GLenum usage_hint) {
        this->add_buffer(GL_ELEMENT_ARRAY_BUFFER, buffer_size, data, usage_hint);
        this->has_ebo = true;
        return *this;
    }

    Vao VaoBuilder::build(GLenum topology_mode, GLsizei topology_size, GLenum topology_type) {
        // If no EBO was added, topology_type will be set to 0 to indicate
        // that draw() should call glDrawArrays instead of glDrawElements.
        GLenum store_type = this->has_ebo ? topology_type : 0;
        return Vao(this->vao_id, topology_mode, topology_size, store_type);
    }

    void VaoBuilder::add_buffer(GLenum target, size_t buffer_size, void* data, GLenum usage_hint) {
        GLuint vbo_id;
        glGenBuffers(1, &vbo_id);
        glBindBuffer(target, vbo_id);
        glBufferData(target, buffer_size, data, usage_hint);
    }
} // namespace vaoz