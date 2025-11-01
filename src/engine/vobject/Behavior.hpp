#pragma once

#include "Component.hpp"

namespace engine {
    class Behavior : public virtual Component {
        public:
            // TODO: On some situations, you can't do some things in theses methods.
            // For instance, it is dangerous to create new VObjects in Awake().
            // That needs to be fixed later.

            virtual void Awake() {}
            virtual void Start() {}
            virtual void Update() {}
            
            std::optional<Behavior*> try_into_behavior() override {
                return this;
            }
    };
}
