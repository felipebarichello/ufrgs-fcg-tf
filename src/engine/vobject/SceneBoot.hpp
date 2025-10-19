#pragma once

#include <vector>
#include "VObject.hpp"

namespace engine {
    class SceneBoot {
        public:
            // TODO: Bake configuration
            virtual void hierarchy(std::vector<VObjectConfig>& root) const = 0;
    };
}
