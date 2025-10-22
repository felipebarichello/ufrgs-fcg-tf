#pragma once
#include "engine/vobject/Component.hpp"
#include "engine/EngineController.hpp"

namespace engine {
    class Drawable : public Component {
    public:
        // Accept the program ID so implementations can query/set uniforms.
        Drawable();
        virtual void draw(GLuint program_id) = 0;
        // TODO: Remember to unsubscribe from EngineController
        virtual ~Drawable() = default;
    };
}