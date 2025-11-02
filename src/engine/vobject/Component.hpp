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

            // Ensure proper destruction through base pointers
            virtual ~Component() {}

            virtual void PreDestroy() {}
            virtual std::optional<Behavior*> try_into_behavior() { return std::nullopt; }
            
            VObject* get_vobject() { return this->vobject_ptr; }
        
        private:
            VObject* vobject_ptr;
    };
}
