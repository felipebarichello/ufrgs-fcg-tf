#pragma once
#include "engine/Drawable.hpp"
#include "engine/vao.hpp"
#include "engine/EngineController.hpp"

class BunnyDrawable : public engine::Drawable {
public:
    BunnyDrawable();
    void draw() override;
};