#pragma once

#include <cstdint>
#include <optional>
#include <iostream>

namespace engine {
    class Behavior;

    /// @brief Base class for all components that can be attached to VObjects.
    class Component {
        public:
            using Id = uint32_t;

            virtual void PreDestroy() {}
            virtual std::optional<Behavior*> try_into_behavior() { return std::nullopt; }

            Id get_id() {
                static Component::Id component_id = Component::s_next_component_id++;
                std::cout << "Generated component ID: " << component_id << std::endl; // TODO: Remove
                return component_id;
            };

        private:
            static Component::Id s_next_component_id;
    };

    class ComponentBuilder {
        public:
            /// @brief Builds a new component instance.
            /// @return A heap pointer to the newly created component.
            virtual Component* build() const = 0;
    };
}
