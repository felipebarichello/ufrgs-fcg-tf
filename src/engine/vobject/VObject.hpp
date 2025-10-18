#pragma once

#include <vector>
#include "Component.hpp"

namespace engine {
    class Scene; // Forward declaration

    class VObject {
        public:
            using Id = uint64_t;

            VObject(Scene* scene, VObject::Id id)
                : scene(scene), id(id) {}

            VObject::Id get_id() const {
                return this->id;
            }

            void awaken();
            void destroy();

            template<std::derived_from<Component> ComponentT>
            void add_component(const ComponentT::Builder& component_builder) {
                this->components.push_back(component_builder.build());
            }

        private:
            Scene* scene;
            VObject::Id id;
            std::vector<VObject*> children;
            std::vector<Component*> components;
    };
}
