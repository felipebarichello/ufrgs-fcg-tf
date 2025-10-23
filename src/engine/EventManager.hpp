#pragma once

#include <vector>
#include <functional>

// TODO: Eliminate
namespace engine {
    using UpdateHandler = std::function<void()>;

    // TODO: Restrict access modifiers?
    class EventManager {
        private:
            std::vector<UpdateHandler> update_subscribers;

        public:
            EventManager() = default;
            void subscribe_update(UpdateHandler handler);
            void update();
    };
}
