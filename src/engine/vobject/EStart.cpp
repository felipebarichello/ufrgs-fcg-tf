#include "EStart.hpp"
#include "../engine_controller.hpp"
#include <iostream>
#include <functional>

namespace engine {
    EStart::EStart() {
        // this->unsubscriber = EngineController::get_events().subscribe_start(EStart::Start);
        EngineController::get_events().subscribe_start(std::bind(&EStart::Start, this));
    }

    EStart::~EStart() {
        // TODO
    }
} // namespace engine
