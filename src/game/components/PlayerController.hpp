#pragma once

#include <engine>
#include <memory>

using engine::Component;
using engine::Behavior;

namespace game::components {
    class PlayerController : public Behavior {
        public:
            PlayerController() = default;
            void Update() override;
    };
} // namespace game::components
