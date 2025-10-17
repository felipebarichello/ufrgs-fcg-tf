#include "VObject.hpp"
#include "Scene.hpp"

namespace engine {
    void VObject::destroy() {
        // Tell the components they are being destroyed so they can clean up
        for (Component* component : this->components) {
            component->PreDestroy();
        }

        // Destroy all children
        for (VObject* child : this->children) {
            child->destroy();
        }

        this->scene->remove_vobject(this);
    }
}
