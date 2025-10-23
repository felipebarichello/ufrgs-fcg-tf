#pragma once

#include <vector>
#include <engine/vobject/Component.hpp>
#include <engine/vobject/Transform.hpp>
#include <engine/math/linalg.hpp>

namespace engine {
    class TransformBuilder {
        public:
            Transform inner;

            Transform& build() {
                return inner;
            }

            TransformBuilder& position(Vec3 p) {
                inner.position() = p;
                return *this;
            }

            TransformBuilder& scale(Vec3 s) {
                inner.scale() = s;
                return *this;
            }

            TransformBuilder& scale(float s) {
                inner.scale() = Vec3(s, s, s);
                return *this;
            }

            TransformBuilder& rotation(const Quaternion& q) {
                inner.quaternion() = q;
                return *this;
            }
    };

    // TODO: Better interface for building VObjects (if possible, use unique_ptrs)
    class VObjectConfig {
        public:
            Transform _transform;
            std::vector<Component*> components;
            std::vector<VObjectConfig> children;

            VObjectConfig() = default;

            VObjectConfig& transform(TransformBuilder transform) {
                this->_transform = transform.build();
                return *this;
            }

            /// @brief For convenience, you don't need to use std::unique_ptr directly.
            /// The conversion is done for you. But be careful with ownership!
            /// The component passed as argument should not be managed elsewhere.
            /// @param component 
            /// @return 
            VObjectConfig& component(Component* component) {
                this->components.push_back(component);
                return *this;
            }

            VObjectConfig& child(VObjectConfig child_config) {
                this->children.push_back(child_config);
                return *this;
            }
    };

    struct SceneRoot {
        std::vector<VObjectConfig> root_vobjects;

        SceneRoot() = default;

        SceneRoot& vobject(VObjectConfig vobject_config) {
            this->root_vobjects.push_back(vobject_config);
            return *this;
        }
    };
}
