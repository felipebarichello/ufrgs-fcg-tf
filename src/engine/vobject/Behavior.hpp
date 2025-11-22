#pragma once

#include "Component.hpp"

namespace engine {
    class Behavior : public virtual Component {
        public:
            void call_awake() { this->Awake(); }
            void call_start() { this->Start(); }

            void call_update() {
                if (this->enabled) {
                    this->Update();
                }
            }

            void call_post_update() {
                if (this->enabled) {
                    this->PostUpdate();
                }
            }

            bool is_enabled() const { return this->enabled; }

            void enable() {
                this->enabled = true;
                this->OnEnable();
            }

            void disable() {
                this->enabled = false;
                this->OnDisable();
            }

            std::optional<Behavior*> try_into_behavior() override {
                return this;
            }

        protected:
            bool enabled = true;

            // TODO: On some situations, you can't do some things in theses methods.
            // For instance, it is dangerous to create new VObjects in Awake().
            // That needs to be fixed later.

            /// @brief Called when the Behavior is first attached to a VObject.
            /// Or after all VObjects are first instantiated in a scene.
            virtual void Awake() {}

            /// @brief Called before the first Update() call.
            virtual void Start() {}

            /// @brief Called every frame if the Behavior is enabled.
            virtual void Update() {}

            /// @brief Called every frame after all Update() calls for that frame if the Behavior is enabled.
            virtual void PostUpdate() {}

            /// @brief Called just after enabling the Behavior.
            virtual void OnEnable() {}

            /// @brief Called just after disabling the Behavior.
            virtual void OnDisable() {}
    };
}
