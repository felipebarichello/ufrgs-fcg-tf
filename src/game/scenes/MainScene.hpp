#pragma once

#include <vector>
#include <engine>

using engine::VObjectConfig;

namespace game::scenes {
    class MainScene : public engine::SceneBoot {
        public:
            void hierarchy(std::vector<VObjectConfig>& root) override;
    };
} // namespace game::scenes
