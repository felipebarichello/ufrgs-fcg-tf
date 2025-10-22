#pragma once

#include <vector>
#include "VObject.hpp"

namespace engine {
    class SceneBoot {
        public:
            virtual void hierarchy(SceneRoot& root) = 0;
    };
}
