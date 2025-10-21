#pragma once

#include <engine>
#include <memory>

using engine::Component;
using engine::Behavior;

namespace game::components {
    class PlayerController : public Behavior {
        public:
            PlayerController() = default;

            void Awake() override {
                std::cout << "PlayerController has awaken" << std::endl;
                std::cout << "PlayerController has CID: " << this->get_id() << std::endl;
            }

            void Start() override {
                std::cout << "PlayerController has started" << std::endl;
            }
    };

    class Controller2 : public Behavior {
        public:
            Controller2() = default;

            void Awake() override {
                std::cout << "Controller2 has awaken" << std::endl;
                std::cout << "Controller2 has CID: " << this->get_id() << std::endl;
            }

            void Start() override {
                std::cout << "Controller2 has started" << std::endl;
            }
    };
} // namespace game::components
