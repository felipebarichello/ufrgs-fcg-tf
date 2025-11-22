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
                for (Behavior* behavior : this->awaken_queue) {
                    behavior->call_awake();
                }
                
                this->awaken_queue.clear();
            }

            void schedule_start(Behavior* component) {
                this->start_queue.push_back(component);
            }

            void trigger_start() {
                for (Behavior* behavior : this->start_queue) {
                    behavior->call_start();
                }
                
                this->start_queue.clear();
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
                this->trigger_awaken();
                this->trigger_start();
                this->trigger_update();
            }

        private:
            std::vector<Behavior*> awaken_queue;
            std::vector<Behavior*> start_queue;
            std::vector<Behavior*> update_subscribers;
    };

} // namespace engine
