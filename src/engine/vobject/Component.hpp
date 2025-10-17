#pragma once

namespace engine {
    class Component {
        public:
            virtual void PreDestroy() = 0;
    };
}
