#pragma once
#include <GLFW/glfw3.h>

namespace engine {
    class Drawable {
    public:
        // Accept the program ID so implementations can query/set uniforms.
        virtual void draw(GLuint program_id) = 0;
        virtual ~Drawable() = default;
    };
}