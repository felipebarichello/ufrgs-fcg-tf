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

namespace engine {

    struct ObjModel {
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        // Carrega um modelo OBJ do disco
        ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
    };

    // Computa normais por vértice se o modelo não tiver normais
    void ComputeNormals(ObjModel* model);

    class ObjDrawable : public Drawable {
    public:
        ObjDrawable(std::string obj_filename);
        void draw(GLuint program_id) override;

        void set_diffuse_reflectance(glm::vec3 reflectance);
        void set_specular_reflectance(glm::vec3 reflectance);
        void set_enviornment_reflectance(glm::vec3 reflectance);
        void set_specular_exponent(float exponent);

    private:
        engine::Vao build_obj_vao(ObjModel* model);
        engine::Vao vao;
        void ComputeNormals(ObjModel* model);
        glm::vec3 diffuse_reflectance{0.8f, 0.4f, 0.8f};
        glm::vec3 specular_reflectance{0.8f, 0.8f, 0.8f};
        glm::vec3 enviornment_reflectance{0.04f, 0.2f, 0.4f};
        float specular_exponent{32.0f};
    };

} // namespace engine