#pragma once

#include <engine/vobject/Behavior.hpp>
#include <memory>

using engine::Behavior;

namespace game::components {
    class PlayerController : public Behavior {
        public:
            // static std::unique_ptr<Component> create(const std::string printme_value) {
            //     return std::make_unique<PlayerController>(std::move(printme_value));
            // }
            static Component* create() {
                return new PlayerController();
            }

            void Update() override {
                std::cout << "PlayerController says: " << this->printme << std::endl;
            }


            // TODO: Everything below this should be private but that generates compiler errors

            std::string printme;

            // PlayerController(const std::string& printme_value) {
            //     this->printme = std::move(printme_value);
            // }

            PlayerController() = default;
    };
} // namespace game::components
