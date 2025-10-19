#pragma once
#include <GLFW/glfw3.h>

namespace engine {
    class Drawable {
    public:
        virtual void draw(GLint model_uniform) = 0;
    };
}