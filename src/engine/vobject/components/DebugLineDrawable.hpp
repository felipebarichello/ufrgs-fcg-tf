#pragma once
#include "Drawable.hpp"
#include <functional>
#include <engine/math/linalg.hpp>

namespace engine {
    class DebugLineDrawable : public Drawable {
    public:
        // Two getters that return world-space positions for the line endpoints
        DebugLineDrawable(std::function<Vec3()> p1_getter, std::function<Vec3()> p2_getter, engine::Vec4 color = engine::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        ~DebugLineDrawable();
        void draw() override;

    private:
        std::function<Vec3()> p1_getter;
        std::function<Vec3()> p2_getter;
        engine::Vec4 color;

        GLuint vao = 0;
        GLuint vbo = 0;
    };
}
