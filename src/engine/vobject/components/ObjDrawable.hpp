#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

namespace engine {

    class ObjDrawable : public engine::Drawable {
        public:
            ObjDrawable(std::string file_name);
            ObjDrawable(std::string file_name, engine::EngineController::ShaderType shader);
            ObjDrawable(std::string file_name, std::string texture_file_name, engine::EngineController::ShaderType shader);
            ObjDrawable(std::string file_name, std::string texture_file_name);
            void draw() override;
            void set_visible();
            void set_invisible();

        private:
            engine::EngineController::ShaderType shader_type { engine::EngineController::ShaderType::Planet };
            bool visible = true;
    };

} // namespace engine