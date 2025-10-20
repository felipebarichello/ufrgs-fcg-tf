#include "ObjLoader.hpp"
#include <cassert>

ObjDrawable::ObjDrawable(std::string obj_filename) {
    ObjModel model = ObjModel(obj_filename.c_str(), nullptr, true);
    this->vao = build_obj_vao(&model);
}

void ObjDrawable::draw(GLint model_uniform) {
    this->vao.draw();
}

engine::Vao ObjDrawable::build_obj_vao(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(static_cast<GLuint>(first_index + 3*triangle + vertex));

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                if ( idx.normal_index != -1 ) {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 ) {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }
    }

    engine::VaoBuilder builder;

    if (!model_coefficients.empty())
        builder.add_vbo(0, 4, model_coefficients.size() * sizeof(float), model_coefficients.data(), GL_STATIC_DRAW);
    if (!normal_coefficients.empty())
        builder.add_vbo(1, 4, normal_coefficients.size() * sizeof(float), normal_coefficients.data(), GL_STATIC_DRAW);
    if (!texture_coefficients.empty())
        builder.add_vbo(2, 2, texture_coefficients.size() * sizeof(float), texture_coefficients.data(), GL_STATIC_DRAW);
    if (!indices.empty())
        builder.add_ebo(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    return builder.build(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT);
}
