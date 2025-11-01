#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class ObjDrawable : public engine::Drawable {
    public:
        ObjDrawable(std::string file_name);
        ObjDrawable(std::string file_name, bool use_phong_shading);
        ObjDrawable(std::string file_name, std::string texture_file_name);
        ~ObjDrawable();
        void draw() override;
    private:
        glm::vec3 Kd{0.8f, 0.4f, 0.8f};
        glm::vec3 Ks{0.8f, 0.8f, 0.8f};
        glm::vec3 Ka{0.04f, 0.2f, 0.4f};
        float Ns{32.0f};
        bool use_phong_shading{false};
};
