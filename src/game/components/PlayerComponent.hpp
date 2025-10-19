#pragma once

#include <engine/vobject/Component.hpp>
#include <memory>

using engine::Component;
using engine::ComponentBuilder;

namespace game::components {
    class PlayerComponent : public Component {
        public:
    };

    class PlayerComponentBuilder : public ComponentBuilder {
        public:
            std::unique_ptr<Component> build() const override {
                return std::make_unique<PlayerComponent>();
            }
    };
} // namespace game::components
