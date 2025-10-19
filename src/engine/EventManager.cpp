// Implementation for EventManager declared in event_manager.hpp

#include "EventManager.hpp"

namespace engine {

    void EventManager::subscribe_start(StartHandler handler) {
        if (this->calling_start_handlers) {
            // If we're currently calling start handlers, call this one immediately
            // TODO: Is this a problem?
            handler();
            return;
        }
        this->start_subscribers.push_back(handler);
    }

    void EventManager::subscribe_update(UpdateHandler handler) {
        this->update_subscribers.push_back(handler);
    }

    void EventManager::update() {
        this->calling_start_handlers = true;
        for (auto& handler : this->start_subscribers) {
            handler(); // Start pending handlers
        }

        this->calling_start_handlers = false;
        this->start_subscribers.clear(); // Clear after running all start handlers
        
        for (auto& handler : this->update_subscribers) {
            handler();
        }
    }

}
