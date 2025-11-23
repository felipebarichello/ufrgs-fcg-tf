#include "SceneScheduler.hpp"

namespace engine {
    
    void SceneScheduler::trigger_awaken() {
        size_t i = 0;
        size_t length = this->awaken_queue.size();

        // The do-while is necessary because new VObjects might be scheduled during Awake calls
        while(true) {
            for (i = 0; i < length; i++) {
                this->awaken_queue[i]->call_awake();
            }

            if (i >= this->awaken_queue.size()) {
                break;
            }

            // If reaches here, a VObject was instanced during Awake calls.
            // Remove already awakened components from the queue and continue.
            this->awaken_queue.erase(this->awaken_queue.begin(), this->awaken_queue.begin() + length);
            length = this->awaken_queue.size();
        };
        
        this->awaken_queue.clear();
    }

    bool SceneScheduler::trigger_start() {
        size_t i = 0;
        size_t length = this->start_queue.size();

        for (i = 0; i < length; i++) {
            this->start_queue[i]->call_start();
        }

        if (i < this->start_queue.size()) {
            // A VObject was instanced during Start calls.
            // Remove already started components from the queue and quit.
            this->start_queue.erase(this->start_queue.begin(), this->start_queue.begin() + length);
            return true;
        }
        
        this->start_queue.clear();
        return false;
    }

    void SceneScheduler::trigger_update() {
        for (size_t i = 0; i < this->update_subscribers.size(); i++) {
            Behavior* behavior = this->update_subscribers[i];
            behavior->call_update();
        }

        for (size_t i = 0; i < this->update_subscribers.size(); i++) {
            Behavior* behavior = this->update_subscribers[i];
            behavior->call_post_update();
        }
    }

    void SceneScheduler::update() {
        bool new_vobjects = false;

        do {
            this->trigger_awaken();
            new_vobjects = this->trigger_start();
        } while (new_vobjects);

        // Don't worry about new VObjects here.
        // They won't run Update until the next frame.
        this->trigger_update();
    }

} // namespace engine
