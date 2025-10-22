#pragma once
#include <GLFW/glfw3.h>
#include "engine/vobject/Component.hpp"

namespace engine {
    class Drawable : public Component {
    public:
        // Accept the program ID so implementations can query/set uniforms.
        virtual void draw(GLuint program_id) = 0;
        virtual ~Drawable() = default;
    };
}