#pragma once

namespace engine {
    class EUpdate {
        public:
            virtual void Update() = 0;

        protected:
            EUpdate();
            ~EUpdate();
    };
} // namespace engine