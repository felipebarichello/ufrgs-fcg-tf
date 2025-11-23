#pragma once

#include "Behavior.hpp"
#include <vector>

namespace engine {
    // TODO: Unsubscribe methods on destroy
    class SceneScheduler {
        public:
            SceneScheduler() = default;

            void schedule_awaken(Behavior* component) {
                this->awaken_queue.push_back(component);
            }

            void trigger_awaken() {
                size_t i = 0;
                size_t length = this->awaken_queue.size();

                // The do-while is necessary because new VObjects might be scheduled during Awake calls
                do {
                    for (i = 0; i < length; i++) {
                        this->awaken_queue[i]->call_awake();
                    }
                } while (i < this->awaken_queue.size());
                
                this->awaken_queue.clear();
            }

            void schedule_start(Behavior* component) {
                this->start_queue.push_back(component);
            }

            /// @brief Triggers Start calls for all scheduled components.
            /// @return If any new components were scheduled during Start calls.
            bool trigger_start() {
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

            void subscribe_update(Behavior* component) {
                this->update_subscribers.push_back(component);
            }

            void trigger_update() {
                for (Behavior* behavior : this->update_subscribers) {
                    behavior->call_update();
                }

                for (Behavior* behavior : this->update_subscribers) {
                    behavior->call_post_update();
                }
            }

            void update() {
                bool new_vobjects = false;

                do {
                    this->trigger_awaken();
                    new_vobjects = this->trigger_start();
                } while (new_vobjects);

                // Don't worry about new VObjects here.
                // They won't run Update until the next frame.
                this->trigger_update();
            }

        private:
            std::vector<Behavior*> awaken_queue;
            std::vector<Behavior*> start_queue;
            std::vector<Behavior*> update_subscribers;
    };

} // namespace engine
