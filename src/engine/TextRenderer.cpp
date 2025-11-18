#include "TextRenderer.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <dejavufont.h>

namespace engine {

    TextRenderer::TextRenderer() : initialized(false) {
        init_text_rendering();
    }

    TextRenderer::~TextRenderer() {
        if (initialized) {
            glDeleteVertexArrays(1, &text_vao);
            glDeleteBuffers(1, &text_vbo);
            glDeleteTextures(1, &font_texture);
        }
    }

    void TextRenderer::init_text_rendering() {
        // Load and compile text shaders
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        
        const char* vertex_shader_code = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";
        
        const char* fragment_shader_code = R"(
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
        
        glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
        glCompileShader(vertex_shader);
        
        glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
        glCompileShader(fragment_shader);
        
        text_program_id = glCreateProgram();
        glAttachShader(text_program_id, vertex_shader);
        glAttachShader(text_program_id, fragment_shader);
        glLinkProgram(text_program_id);
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        
        // Create font texture from embedded dejavufont
        glGenTextures(1, &font_texture);
        glBindTexture(GL_TEXTURE_2D, font_texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            dejavufont.tex_width,
            dejavufont.tex_height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            dejavufont.tex_data
        );
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Configure VAO/VBO for texture quads
        glGenVertexArrays(1, &text_vao);
        glGenBuffers(1, &text_vbo);
        glBindVertexArray(text_vao);
        glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        // Build character map from dejavufont
        for (size_t i = 0; i < dejavufont.glyphs_count; i++) {
            const texture_glyph_t& glyph = dejavufont.glyphs[i];
            Character character = {
                font_texture,
                glm::ivec2(glyph.width, glyph.height),
                glm::ivec2(glyph.offset_x, glyph.offset_y),
                static_cast<GLuint>(glyph.advance_x * 64)
            };
            characters.insert(std::pair<char, Character>(static_cast<char>(glyph.codepoint), character));
        }
        
        initialized = true;
    }

    void TextRenderer::draw() {
        // This drawable doesn't auto-draw; use render_text() instead
    }

    void TextRenderer::render_text(std::string text, float x, float y, float scale, glm::vec3 color) {
        if (!initialized) return;
        
        // Save OpenGL state
        GLboolean depth_test_enabled;
        glGetBooleanv(GL_DEPTH_TEST, &depth_test_enabled);
        GLboolean blend_enabled;
        glGetBooleanv(GL_BLEND, &blend_enabled);
        GLint blend_src, blend_dst;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst);
        
        // Setup for text rendering
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        
        glUseProgram(text_program_id);
        
        // Get window size for projection matrix
        GLFWwindow* window = EngineController::get_instance()->get_window();
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window_width), 
                                          0.0f, static_cast<float>(window_height));
        glUniformMatrix4fv(glGetUniformLocation(text_program_id, "projection"), 
                          1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(text_program_id, "textColor"), 
                   color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(text_vao);
        
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            auto it = characters.find(*c);
            if (it == characters.end()) continue;
            
            const texture_glyph_t* glyph = nullptr;
            for (size_t i = 0; i < dejavufont.glyphs_count; i++) {
                if (dejavufont.glyphs[i].codepoint == static_cast<uint32_t>(*c)) {
                    glyph = &dejavufont.glyphs[i];
                    break;
                }
            }
            
            if (!glyph) continue;
            
            float xpos = x + glyph->offset_x * scale;
            float ypos = y - (glyph->height - glyph->offset_y) * scale;
            float w = glyph->width * scale;
            float h = glyph->height * scale;
            
            // Update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   glyph->s0, glyph->t0 },
                { xpos,     ypos,       glyph->s0, glyph->t1 },
                { xpos + w, ypos,       glyph->s1, glyph->t1 },
                
                { xpos,     ypos + h,   glyph->s0, glyph->t0 },
                { xpos + w, ypos,       glyph->s1, glyph->t1 },
                { xpos + w, ypos + h,   glyph->s1, glyph->t0 }
            };
            
            glBindTexture(GL_TEXTURE_2D, font_texture);
            glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            x += glyph->advance_x * scale;
        }
        
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Restore OpenGL state
        if (depth_test_enabled) {
            glEnable(GL_DEPTH_TEST);
        }
        if (!blend_enabled) {
            glDisable(GL_BLEND);
        } else {
            glBlendFunc(blend_src, blend_dst);
        }
    }

} // namespace engine
