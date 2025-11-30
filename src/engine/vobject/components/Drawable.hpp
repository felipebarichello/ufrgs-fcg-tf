#pragma once
#include "engine/vobject/Component.hpp"
#include "engine/EngineController.hpp"
#include "vobject/VObject.hpp"
#include "vao.hpp"

namespace engine {
    class Drawable : public virtual Component {
        public:
            Drawable();
            virtual ~Drawable();
            virtual void draw() = 0;
            // Return true if this drawable should be rendered in the transparent
            // pass (after opaque geometry). Default is false.
            virtual bool is_transparent() const { return false; }
        protected:
            Vao* vao_ptr;
    };
}