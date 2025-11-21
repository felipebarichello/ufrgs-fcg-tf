#pragma once

#include <vector>
#include <engine>

#define MAIN_PLANET_RADIUS 200.0f
#define PLANET_1_RADIUS 50.0f
#define PLANET_2_RADIUS 60.0f
#define PLANET_3_RADIUS 150.0f
#define PLANET_4_RADIUS 120.0f
#define PLANET_5_RADIUS 80.0f
#define PLANET_6_RADIUS 130.0f
#define PLANET_7_RADIUS 50.0f
#define PLANET_8_RADIUS 100.0f

using engine::VObjectConfig;
using engine::SceneRoot;

namespace game::scenes {
    class MainScene : public engine::SceneBoot {
        public:
            void hierarchy(SceneRoot& root) override;
    };
} // namespace game::scenes
