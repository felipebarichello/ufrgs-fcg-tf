#include "VObject.hpp"
#include "Scene.hpp"
#include "Behavior.hpp"
#include "Component.hpp"
#include <memory>

namespace engine {
    void VObject::add_component(Component* component) {
        Component::Id component_id = component->get_id();
        component->vobject_ptr = this;
        this->components.emplace(component_id, component);

        std::cout << "Added component with CID: " << component_id << " to VObject with VID: " << this->get_id() << std::endl;

        // If the component is a Behavior, schedule its Behavior methods
        if (auto behavior_opt = component->try_into_behavior()) {
            // TODO: Maybe awaken should schedule start and start should subscribe update?
            Behavior* behavior = behavior_opt.value();
            this->scene->scheduler.schedule_awaken(behavior);
            this->scene->scheduler.schedule_start(behavior);
            this->scene->scheduler.subscribe_update(behavior);
        }
    }

    void VObject::destroy() {
        // Tell the components they are being destroyed so they can clean up
        for (auto& item : this->components) {
            item.second->PreDestroy();
        }

        // Destroy all children
        for (VObject* child : this->children) {
            child->destroy();
        }

        this->scene->delete_vobject(this);
    }
}
