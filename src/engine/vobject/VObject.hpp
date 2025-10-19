#pragma once

#include <vector>
#include "Component.hpp"
#include <cstdint>
#include <span>
#include <unordered_map>

namespace engine {
    class Scene; // Forward declaration

    // CRITICAL: This class manages lifetimes of components and children.
    class VObject {
        public:
            using Id = uint64_t;

            friend class Scene;

            VObject(Scene* scene, VObject::Id id)
                : scene(scene), id(id) {}

            VObject::Id get_id() const {
                return this->id;
            }

            void add_component(const ComponentBuilder* component_builder);
            
            // TODO: Remove children when destroyed
            // FIXME: Can cause multiple parents
            void add_child(VObject* child) {
                this->children.push_back(child);
            }

            /// @brief CRITICAL: This is a dangerous operation that may cause dangling pointers!
            void destroy(); // TODO: Defer destruction to end of frame?

        private:
            Scene* scene;
            VObject::Id id;
            std::vector<VObject*> children;

            /// @brief List of components attached to this VObject.
            /// CRITICAL: The lifetime of these components is managed by the VObject.
            std::unordered_map<Component::Id, std::unique_ptr<Component>> components;
    };

    struct VObjectConfig {
        std::vector<ComponentBuilder*> components;
        std::vector<VObjectConfig> children;
    };
}
