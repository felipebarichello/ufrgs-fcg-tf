#pragma once
#include "Drawable.hpp"
#include "vao.hpp"
#include "EngineController.hpp"
#include <glm/vec3.hpp>
#include <string>

class TextDrawable : public engine::Drawable {
    public:
    TextDrawable();
    ~TextDrawable();
    void setText(const std::string& text, float scale, glm::vec3 color, float x, float y);
    void draw() override;
    private:
    float x = 0.0f;
    float y = 0.0f;
    float scale = 1.0f;
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    std::string text; 
};
