#include "TextRenderer.hpp"
#include "EngineController.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdio>

namespace engine {

TextRenderer::TextRenderer() 
    : vao(0), vbo(0), shader_program(0), ft(nullptr), face(nullptr) {
}

TextRenderer::~TextRenderer() {
    // Clean up FreeType resources
    if (face) {
        FT_Done_Face(face);
    }
    if (ft) {
        FT_Done_FreeType(ft);
    }
    
    // Clean up OpenGL resources
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (shader_program) glDeleteProgram(shader_program);
    
    // Clean up character textures
    for (auto& pair : characters) {
        glDeleteTextures(1, &pair.second.texture_id);
    }
}

void TextRenderer::init(const char* font_path, unsigned int font_size) {
    init_freetype(font_path, font_size);
    load_shaders();
    
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::init_freetype(const char* font_path, unsigned int font_size) {
    // Initialize FreeType library
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library\n");
        return;
    }

    // Load font face
    if (FT_New_Face(ft, font_path, 0, &face)) {
        fprintf(stderr, "ERROR::FREETYPE: Failed to load font from %s\n", font_path);
        return;
    }

    // Set font size
    FT_Set_Pixel_Sizes(face, 0, font_size);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 ASCII characters
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "ERROR::FREETYPE: Failed to load Glyph for character %c\n", c);
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::load_shaders() {
    // Text vertex shader source
    const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
        out vec2 TexCoords;

        uniform mat4 projection;

        void main() {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    // Text fragment shader source
    const char* fragment_shader_source = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D text;
        uniform vec3 textColor;

        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }
    )";

    // Compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    // Check for vertex shader compile errors
    GLint success;
    GLchar info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", info_log);
    }

    // Compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", info_log);
    }

    // Link shaders to program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", info_log);
    }

    // Clean up shaders as they're linked into our program now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void TextRenderer::render_text(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    // Save current OpenGL state
    GLint last_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLboolean last_blend_enabled = glIsEnabled(GL_BLEND);
    GLboolean last_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLint last_blend_src, last_blend_dst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst);

    // Activate shader
    glUseProgram(shader_program);
    glUniform3f(glGetUniformLocation(shader_program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // Enable blending for text transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth test for HUD overlay
    glDisable(GL_DEPTH_TEST);

    // Set up orthographic projection for screen space rendering
    GLFWwindow* window = EngineController::get_instance()->get_window();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Convert y from top-origin to bottom-origin
    y = height - y;

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = characters[*c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursors for next glyph
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }

    // Restore OpenGL state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (last_depth_test) {
        glEnable(GL_DEPTH_TEST);
    }
    
    if (!last_blend_enabled) {
        glDisable(GL_BLEND);
    } else {
        glBlendFunc(last_blend_src, last_blend_dst);
    }
    
    glUseProgram(last_program);
}

} // namespace engine
