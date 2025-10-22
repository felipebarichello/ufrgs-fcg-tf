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
}
