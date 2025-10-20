#pragma once

#include <engine>
#include <memory>

using engine::Component;
using engine::Behavior;

namespace game::components {
    class PlayerController : public Behavior {
        public:
            // static std::unique_ptr<Component> create(const std::string printme_value) {
            //     return std::make_unique<PlayerController>(std::move(printme_value));
            // }
            static Component* create() {
                PlayerController* inst = new PlayerController();
                std::cout << "PlayerController created!" << std::endl
                    << "This PlayerController has CID: " << inst->get_id() << std::endl
                    << "and if I run again: " << inst->get_id() << std::endl;

                PlayerController* other = new PlayerController();
                std::cout << "Another PlayerController has CID: " << other->get_id() << std::endl;

                Component* cast = (Component*)inst;
                std::cout << "Casted PlayerController to Component with CID: " << cast->get_id() << std::endl;

                return inst;
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
