#pragma once
#include <tiny_obj_loader.h>
#include <string>
#include <stdexcept>
#include <set>
#include <vector>
#include <glm/glm.hpp>
#include "vao.hpp"
#include "math/matrices.hpp"
#include "Drawable.hpp"
#include <glm/gtc/type_ptr.hpp>

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
        ObjLoader() = default;
        Vao* load(const char* filename, const char* basepath = NULL, bool triangulate = true);
    private:
        std::unordered_map<std::string, Vao*> loaded_vaos;
        engine::Vao build_obj_vao(ObjModel* model);
        void ComputeNormals(ObjModel* model);
    };
} // namespace engine