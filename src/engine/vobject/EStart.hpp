#pragma once

namespace engine {
    class EStart {
        public:
            virtual void Start() = 0;

        protected:
            EStart();
            ~EStart();
    };
} // namespace engine