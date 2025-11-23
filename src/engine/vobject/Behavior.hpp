#pragma once

#include "Component.hpp"

namespace engine {
    class Behavior : public virtual Component {
        public:
            void call_awake() { this->Awake(); }

            void call_start() {
                this->Start();

                this->started = true;
                if (this->enabled) {
                    this->should_update = true;
                }
            }

            void call_update() {
                if (this->should_update) {
                    this->Update();
                }
            }

            void call_post_update() {
                if (this->should_update) {
                    this->PostUpdate();
                }
            }

            bool is_enabled() const { return this->enabled; }

            void enable() {
                this->enabled = true;

                if (this->started) {
                    this->should_update = true;
                }

                this->OnEnable();
            }

            void disable() {
                this->enabled = false;
                this->should_update = false;
                this->OnDisable();
            }

            std::optional<Behavior*> try_into_behavior() override {
                return this;
            }

        protected:
            bool enabled = true;
            bool started = false;
            bool should_update = false;

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
