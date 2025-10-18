#include "VObject.hpp"
#include "Scene.hpp"
#include "Behavior.hpp"
#include "Component.hpp"

namespace engine {
    void VObject::add_component(const ComponentBuilder* component_builder) {
        Component* component = component_builder->build();
        Component::Id component_id = component->get_id();
        this->components.emplace(component_id, component);

        // If the component is a Behavior, schedule its Behavior methods
        if (auto behavior_opt = component->try_into_behavior()) {
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
