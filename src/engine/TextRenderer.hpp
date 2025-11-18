#pragma once

#include <engine>
#include <map>
#include <string>

namespace engine {
    struct Character {
        GLuint texture_id;
        glm::ivec2 size;
        glm::ivec2 bearing;
        GLuint advance;
    };

    class TextRenderer : public Drawable {
        public:
            TextRenderer();
            ~TextRenderer();
            void draw() override;
            void render_text(std::string text, float x, float y, float scale, glm::vec3 color);
            
        private:
            void init_text_rendering();
            std::map<char, Character> characters;
            GLuint text_vao, text_vbo;
            GLuint text_program_id;
            GLuint font_texture;
            bool initialized;
    };
} // namespace engine
