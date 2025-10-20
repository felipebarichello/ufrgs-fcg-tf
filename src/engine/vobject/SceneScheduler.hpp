#pragma once

#include "Behavior.hpp"
#include <vector>

namespace engine {
    // TODO: Unsubscribe methods on destroy
    class SceneScheduler {
        public:
            // TODO: Subscribe to other events
            static SceneScheduler create();

            void schedule_awaken(Behavior* component) {
                this->awaken_queue.push_back(component);
            }

            void trigger_awaken() {
                for (Behavior* behavior : this->awaken_queue) {
                    behavior->Awake();
                }
                
                this->awaken_queue.clear();
            }

            void schedule_start(Behavior* component) {
                this->start_queue.push_back(component);
            }

            void trigger_start() {
                for (Behavior* behavior : this->start_queue) {
                    behavior->Start();
                }
                
                this->start_queue.clear();
            }

            void subscribe_update(Behavior* component) {
                this->update_subscribers.push_back(component);
            }

            void trigger_update() {
                for (Behavior* behavior : this->update_subscribers) {
                    behavior->Update();
                }
            }

        private:
            std::vector<Behavior*> awaken_queue;
            std::vector<Behavior*> start_queue;
            std::vector<Behavior*> update_subscribers;

            SceneScheduler() = default;
    };

} // namespace engine
