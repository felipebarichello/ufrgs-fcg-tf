#include "VObject.hpp"
#include "Scene.hpp"
#include "Behavior.hpp"

namespace engine {
    void VObject::awaken() {
        // Call Awake on all Behavior components
        for (Component* component : this->components) {
            component->try_into_behavior().and_then([](Behavior* behavior) -> std::optional<Behavior*> {
                behavior->Awake();
                return std::nullopt;
            });
        }
    }

    void VObject::destroy() {
        // Tell the components they are being destroyed so they can clean up
        for (Component* component : this->components) {
            component->PreDestroy();
        }

        // Destroy all children
        for (VObject* child : this->children) {
            child->destroy();
        }

        this->scene->delete_vobject(this);
    }
}
