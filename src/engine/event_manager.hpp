#pragma once

#include <vector>
#include "vobject/EStart.hpp"
#include <functional>

using engine::EStart;

namespace engine {
    using StartHandler  = std::function<void()>;
    using UpdateHandler = std::function<void()>;

    // TODO: Restrict access modifiers?
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
