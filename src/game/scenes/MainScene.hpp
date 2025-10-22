#pragma once

#include <vector>
#include <engine>

using engine::VObjectConfig;
using engine::SceneRoot;

namespace game::scenes {
    class MainScene : public engine::SceneBoot {
        public:
            void hierarchy(SceneRoot& root) override;
    };
} // namespace game::scenes
