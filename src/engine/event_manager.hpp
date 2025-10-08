#pragma once

#include <vector>

namespace engine {
    using StartHandler  = void (*)();
    using UpdateHandler = void (*)();

    class EventManager {
        private:
            std::vector<StartHandler>  start_subscribers;
            std::vector<UpdateHandler> update_subscribers;
            bool calling_start_handlers = false;

        public:
            EventManager() = default;
            void subscribe_start(StartHandler handler);
            void subscribe_update(UpdateHandler handler);
            void update();
    };
}
