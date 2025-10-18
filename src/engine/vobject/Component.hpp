#pragma once

#include <optional>

namespace engine {
    class Behavior;

    class Component {
        public:
            virtual void PreDestroy() {}
            virtual std::optional<Behavior*> try_into_behavior() { return std::nullopt; }
    };
}
