#include "BunnyDrawable.hpp"
#include "engine/ObjLoader.hpp"
#include <macros>

using namespace engine;

BunnyDrawable::BunnyDrawable() {
    this->vao_ptr = EngineController::get_obj_loader()->load("assets/models/bunny.obj");
}

void BunnyDrawable::draw() {
    if (this->vao_ptr) {
        this->vao_ptr->draw();
    }
}