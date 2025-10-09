#include "EUpdate.hpp"
#include <iostream>
#include <functional>
#include "../engine_controller.hpp"

// TODO: Make it similar to EStart
namespace engine {
    EUpdate::EUpdate() {
        EngineController::get_events().subscribe_update(std::bind(&EUpdate::Update, this));
    }

    EUpdate::~EUpdate() {
        // TODO
    }
} // namespace engine
