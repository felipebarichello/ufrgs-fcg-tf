#pragma once

#include <cstdint>
#include <optional>
#include <iostream>
#include <memory>

namespace engine {
    // Forward declarations
    class Behavior;
    class VObject;

    /// @brief Base class for all components that can be attached to VObjects.
    ///
    /// CRITICAL: Do not do Component-specific logic in constructors
    /// In fact, avoid defining constructors in Component implementors.
    class Component {
        public:
            using Id = uint32_t;
            
            friend VObject;

            Component() {
                std::cout << "Component created" << std::endl; // TODO: Remove
            }

            virtual void PreDestroy() {}
            virtual std::optional<Behavior*> try_into_behavior() { return std::nullopt; }

            Component::Id get_id() {
                static Component::Id component_id = Component::next_component_id++;
                std::cout << "Generated component ID: " << component_id << std::endl; // TODO: Remove
                return component_id;
            };
            
            VObject* vobject() { return this->vobject_ptr; }
        
        private:
            static Component::Id next_component_id;
            VObject* vobject_ptr;
    };
}
