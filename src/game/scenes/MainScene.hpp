#pragma once

#include <vector>
#include <engine>

namespace game::scenes {
    class MainScene : public engine::SceneBoot {
        public:
            void hierarchy(std::vector<engine::VObjectConfig>& root) const override {
                (void) root;
            }
    };
} // namespace game::scenes
