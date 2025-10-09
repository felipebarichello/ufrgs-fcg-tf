#include "EStart.hpp"
#include "../engine_controller.hpp"
#include <iostream>

namespace engine {
    EStart::EStart() {
        // this->unsubscriber = EngineController::get_events().subscribe_start(EStart::Start);
        // EngineController::get_events().subscribe_start(&EStart::Start);
    }

    EStart::~EStart() {
        // TODO
    }
} // namespace engine
