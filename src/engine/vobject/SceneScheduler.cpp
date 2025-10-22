#include "SceneScheduler.hpp"
#include <engine/EngineController.hpp>

using engine::EngineController;

namespace engine {
    SceneScheduler::SceneScheduler() : awaken_queue(), start_queue(), update_subscribers() {
        auto& events = engine::EngineController::get_events();
        events.subscribe_update(std::bind(&SceneScheduler::update, this));
    }
} // namespace engine
