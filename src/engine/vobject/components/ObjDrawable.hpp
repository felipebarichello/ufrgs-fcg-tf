#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class ObjDrawable : public engine::Drawable {
    public:
    ObjDrawable(std::string file_name);
    ObjDrawable(std::string file_name, engine::EngineController::ShaderType shader);
    ObjDrawable(std::string file_name, std::string texture_file_name);
    ObjDrawable(std::string file_name, std::string texture_file_name, engine::EngineController::ShaderType shader);
        ~ObjDrawable();
        void draw() override;
    private:
        glm::vec3 Kd{1.0f, 1.0f, 1.0f};
        glm::vec3 Ks{0.1f, 0.1f, 0.1f};
        glm::vec3 Ka{0.0f, 0.0f, 0.0f};
        float Ns{32.0f};
    engine::EngineController::ShaderType shader_type{ engine::EngineController::ShaderType::Gouraud };
};
