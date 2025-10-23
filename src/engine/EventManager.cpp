// Implementation for EventManager declared in event_manager.hpp

#include "EventManager.hpp"

namespace engine {

    void EventManager::subscribe_update(UpdateHandler handler) {
        this->update_subscribers.push_back(handler);
    }

    void EventManager::update() {
        for (auto& handler : this->update_subscribers) {
            handler();
        }
    }

}
