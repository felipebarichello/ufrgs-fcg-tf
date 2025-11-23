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

            void trigger_awaken();

            void schedule_start(Behavior* component) {
                this->start_queue.push_back(component);
            }

            /// @brief Triggers Start calls for all scheduled components.
            /// @return If any new components were scheduled during Start calls.
            bool trigger_start();

            void subscribe_update(Behavior* component) {
                this->update_subscribers.push_back(component);
            }

            void trigger_update();

            void update();

        private:
            std::vector<Behavior*> awaken_queue;
            std::vector<Behavior*> start_queue;
            std::vector<Behavior*> update_subscribers;
    };

} // namespace engine
