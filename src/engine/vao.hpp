// #include "lib/lib.hpp"
/* --- FLATTEN: lib/lib.hpp --- */
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3

namespace vao {
    typedef struct Position {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;

        Position();
        Position(GLfloat x, GLfloat y, GLfloat z);
        Position(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    } Position;

    typedef struct Color {
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;

        Color();
        Color(GLfloat r, GLfloat g, GLfloat b);
        Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    } Color;

    typedef struct Vertex {
        Position position;
        Color color;

        Vertex(Position position, Color color);
    } Vertex;

    template<size_t N>
    class GeometryEditor {
    private:
        Position (&vertex_positions)[N];
        Color (&vertex_colors)[N];

    public:
        GeometryEditor(Position (&vertex_positions)[N], Color (&vertex_colors)[N])
            : vertex_positions(vertex_positions), vertex_colors(vertex_colors) {}

        void set(size_t index, Vertex vertex) {
            this->vertex_positions[index] = vertex.position;
            this->vertex_colors[index] = vertex.color;
        }
    };

    typedef class Vao {
    private:
        GLuint vao_id;
        GLenum topology_mode;
        GLsizei topology_size;
        GLenum topology_type;
        
    public:
        Vao(GLuint vao_id, GLenum topology_mode, GLsizei topology_size, GLenum topology_type);
        void draw() const;

    private: 
        void bind() const;
        void unbind() const;
    } Vao;

    typedef class VaoBuilder {
    private:
        GLuint vao_id;

    public:
        VaoBuilder();
        ~VaoBuilder();
        VaoBuilder& add_vbo(GLuint location, GLint number_of_dimensions, size_t buffer_size, void* data, GLenum usage_hint);
        VaoBuilder& add_ebo(size_t buffer_size, void* data, GLenum usage_hint);
        Vao build(GLenum topology_mode, GLsizei topology_size, GLenum topology_type);

    private:
        void add_buffer(GLenum target, size_t buffer_size, void* data, GLenum usage_hint);
    } VaoBuilder;
}
