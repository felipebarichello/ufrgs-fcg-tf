#pragma once
#include "engine/vobject/Component.hpp"
#include "engine/EngineController.hpp"
#include "vobject/VObject.hpp"

namespace engine {
    class Drawable : public Component {
    public:
        Drawable();
        ~Drawable();
        virtual void draw() = 0;
    };
}