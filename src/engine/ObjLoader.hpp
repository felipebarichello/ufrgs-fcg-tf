#pragma once
#include <tiny_obj_loader.h>
#include <string>
#include <stdexcept>
#include <set>
#include <glm/glm.hpp>
#include "vao.hpp"
#include "math/matrices.hpp"
#include "Drawable.hpp"

namespace engine {

    struct ObjModel {
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;
        ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
    };

    class ObjDrawable : public Drawable {
        public:
            ObjDrawable(std::string obj_filename);
            void draw(GLint model_uniform);
        private:
            engine::Vao build_obj_vao(ObjModel* model);
            engine::Vao vao; 
    };
}


