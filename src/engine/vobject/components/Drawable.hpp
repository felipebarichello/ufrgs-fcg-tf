#pragma once
#include "engine/vobject/Component.hpp"
#include "engine/EngineController.hpp"
#include "vobject/VObject.hpp"
#include "vao.hpp"

namespace engine {
    class Drawable : public Component {
        public:
            Drawable();
            ~Drawable();
            virtual void draw() = 0;
        protected:
            Vao* vao_ptr;
    };
}