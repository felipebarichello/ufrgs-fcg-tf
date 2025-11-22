#pragma once

#include <vector>
#include "Component.hpp"
#include "Transform.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>

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

            /// @brief Add a child VObject to this VObject.
            /// @param child
            void add_child(VObject* child) {
                // TODO: Remove children when they are destroyed
                // FIXME: Can cause multiple parents

                this->children.emplace(child->get_id(), child);
                child->set_parent_raw(this);
            }

            /// @brief Make child VObject a root VObject
            ///
            /// # Critical
            ///
            /// This causes the ex-child not to be destroyed when `this` is destroyed.
            ///
            /// Transform inheritance will also be lost.
            ///
            /// @param child 
            void disown_child(VObject* child);

            /// @brief # Critical
            ///
            /// This is a dangerous operation that may cause dangling pointers!
            void destroy(); // TODO: Defer destruction to end of frame?

            Transform& transform();
            Scene* get_scene() { return this->scene; }
            std::optional<VObject*> get_parent() { return this->parent; }

        private:
            Scene* scene;
            VObject::Id id;
            std::optional<VObject*> parent = std::nullopt;
            std::unordered_map<VObject::Id, VObject*> children;
            Transform _transform;

            /// @brief List of components attached to this VObject.
            /// CRITICAL: The lifetime of these components is managed by the VObject.
            std::vector<std::unique_ptr<Component>> components;

            /// @brief Sets parent of this VObject.
            /// 
            /// Does not do anything else.
            /// For now, VObjects should be disowned by their parent.
            /// 
            /// @param parent 
            void set_parent_raw(std::optional<VObject*> parent) {
                this->parent = parent;
            }
    };
}
