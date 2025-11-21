#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>

namespace engine {

/// @brief Character information for text rendering
struct Character {
    GLuint texture_id;   // ID handle of the glyph texture
    glm::ivec2 size;     // Size of glyph
    glm::ivec2 bearing;  // Offset from baseline to left/top of glyph
    GLuint advance;      // Horizontal offset to advance to next glyph
};

/// @brief Simple text renderer using FreeType
class TextRenderer {
private:
    std::map<GLchar, Character> characters;
    GLuint vao;
    GLuint vbo;
    GLuint shader_program;
    FT_Library ft;
    FT_Face face;

    void init_freetype(const char* font_path, unsigned int font_size);
    void load_shaders();

public:
    TextRenderer();
    ~TextRenderer();

    /// @brief Initialize the text renderer with a font
    /// @param font_path Path to the TrueType font file
    /// @param font_size Size of the font in pixels
    void init(const char* font_path, unsigned int font_size);

    /// @brief Render text on screen
    /// @param text The text string to render
    /// @param x X position in screen coordinates (0 = left)
    /// @param y Y position in screen coordinates (0 = top)
    /// @param scale Scale factor for the text
    /// @param color RGB color of the text
    void render_text(const std::string& text, float x, float y, float scale, glm::vec3 color);
};

} // namespace engine
