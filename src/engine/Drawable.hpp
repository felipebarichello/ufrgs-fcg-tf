#pragma once
#include <GLFW/glfw3.h>

namespace engine {
    class Drawable {
    public:
        virtual void draw(GLuint program_id) = 0;
    };
}