#pragma once

#include "Component.hpp"

namespace engine {
    class Behavior : public Component {
        public:
            virtual void Awake() {}
            virtual void Start() {}
            virtual void Update() {}
            
            std::optional<Behavior*> try_into_behavior() override {
                return this;
            }
    };
}
