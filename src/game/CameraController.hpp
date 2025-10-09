#pragma once

#include <engine.hpp>

namespace game {
    using engine::EStart;
    using engine::EUpdate;

    class CameraController : public EStart, public EUpdate {
        public:
            CameraController();
            ~CameraController();
            void Start() override;
            void Update() override;
    };
}
