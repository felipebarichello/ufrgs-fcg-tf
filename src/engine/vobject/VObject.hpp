#pragma once

#include <vector>
#include "Component.hpp"
#include "Transform.hpp"
#include <cstdint>
#include <memory>

namespace engine {
    class Scene; // Forward declaration

    // CRITICAL: This class manages lifetimes of components and children.
    class VObject {
        public:
            using Id = uint64_t;

            friend class Scene;

            VObject(Scene* scene, VObject::Id id, Transform transform)
                : scene(scene), id(id), _transform(transform) {}

            VObject(Scene* scene, VObject::Id id)
                : scene(scene), id(id) {}

            VObject::Id get_id() const {
                return this->id;
            }

            void add_component(std::unique_ptr<Component> component);

            /// @brief Adds a child VObject to this VObject.
            /// Should only be called on the parent VObject.
            /// @param child
            void add_child(VObject* child) {
                // TODO: Remove children when they are destroyed
                // FIXME: Can cause multiple parents

                this->children.push_back(child);
                child->set_parent(this);
            }

            /// @brief CRITICAL: This is a dangerous operation that may cause dangling pointers!
            void destroy(); // TODO: Defer destruction to end of frame?

            Transform& transform();

        private:
            Scene* scene;
            VObject::Id id;
            std::optional<VObject*> parent = std::nullopt;
            std::vector<VObject*> children;
            Transform _transform;

            /// @brief List of components attached to this VObject.
            /// CRITICAL: The lifetime of these components is managed by the VObject.
            std::vector<std::unique_ptr<Component>> components;
            
            void set_parent(std::optional<VObject*> parent) {
                this->parent = parent;
            }
    };

    // TODO: Better interface for building VObjects (if possible, use unique_ptrs)
    class VObjectConfig {
        public:
            std::vector<Component*> components;
            std::vector<VObjectConfig> children;

            VObjectConfig() = default;

            /// @brief For convenience, you don't need to use std::unique_ptr directly.
            /// The conversion is done for you. But be careful with ownership!
            /// The component passed as argument should not be managed elsewhere.
            /// @param component 
            /// @return 
            inline VObjectConfig& component(Component* component) {
                this->components.push_back(component);
                return *this;
            }

            inline VObjectConfig& child(VObjectConfig child_config) {
                this->children.push_back(child_config);
                return *this;
            }
    };

    struct SceneRoot {
        std::vector<VObjectConfig> root_vobjects;

        SceneRoot() = default;

        inline SceneRoot& vobject(VObjectConfig vobject_config) {
            this->root_vobjects.push_back(vobject_config);
            return *this;
        }
    };
}
