#include "Drawable.hpp"

using namespace engine;

Drawable::Drawable() {
    EngineController::add_drawable(this);
}

Drawable::~Drawable() {
    EngineController::remove_drawable(this);
}

    