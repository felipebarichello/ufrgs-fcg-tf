#include "SceneScheduler.hpp"
#include <engine/EngineController.hpp>

using engine::EngineController;

namespace engine {
    SceneScheduler SceneScheduler::create() {
        SceneScheduler inst;
        auto events = engine::EngineController::get_events();
        events.subscribe_update(std::bind(&SceneScheduler::trigger_update, &inst));
        return inst;
    }
} // namespace engine
