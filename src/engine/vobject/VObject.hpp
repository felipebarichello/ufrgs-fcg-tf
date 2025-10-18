#pragma once

#include <vector>
#include "Component.hpp"

namespace engine {
    class Scene; // Forward declaration

    using VObjectId = uint64_t;

    class VObject {
        public:
            VObject(Scene* scene, VObjectId id)
                : scene(scene), id(id) {}

            VObjectId get_id() const {
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
            VObjectId id;
            std::vector<VObject*> children;
            std::vector<Component*> components;
    };
}
