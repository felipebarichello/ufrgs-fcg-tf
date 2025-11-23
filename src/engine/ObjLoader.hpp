#pragma once
#include <tiny_obj_loader.h>
#include <string>
#include <stdexcept>
#include <set>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "vao.hpp"
#include "math/linalg.hpp"
#include <engine/vobject/components/Drawable.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace engine {

    struct ObjModel {
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        // Carrega um modelo OBJ do disco
        ObjModel() = default;
        ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
    };

    class ObjLoader {
    public:
        static Vao* load(const char* filename);
        static Vao* load(const char* filename, const char* texture_filename);
        // Return the GL texture object id associated with a texture unit
        static GLuint get_texture_object_for_unit(GLuint unit);
    private:
        static std::unordered_map<std::string, Vao*> loaded_vaos;
        static std::vector<std::string> loaded_texture_filenames;
        static std::vector<GLuint> loaded_texture_objects;
        static engine::Vao build_obj_vao(ObjModel* model);
        static void ComputeNormals(ObjModel* model);
        static GLuint LoadTextureImage(const char* filename);
    };
} // namespace engine