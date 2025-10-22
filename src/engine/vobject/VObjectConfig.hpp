#pragma once

#include <vector>
#include <engine/vobject/Component.hpp>
#include <engine/math/linalg.hpp>

namespace engine {
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
