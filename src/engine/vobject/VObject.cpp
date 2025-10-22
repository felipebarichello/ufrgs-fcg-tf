#include "VObject.hpp"
#include "Scene.hpp"
#include "Behavior.hpp"
#include "Component.hpp"
#include <memory>

namespace engine {
    void VObject::add_component(std::unique_ptr<Component> component) {
        component->vobject_ptr = this;

        // If the component is a Behavior, schedule its Behavior methods
        if (auto behavior_opt = component->try_into_behavior()) {
            // TODO: Maybe awaken should schedule start and start should subscribe update?
            Behavior* behavior = behavior_opt.value();
            this->scene->scheduler.schedule_awaken(behavior);
            this->scene->scheduler.schedule_start(behavior);
            this->scene->scheduler.subscribe_update(behavior);
        }

        this->components.push_back(std::move(component));
    }

    void VObject::destroy() {
        // Tell the components they are being destroyed so they can clean up
        for (auto& item : this->components) {
            item->PreDestroy();
        }

        // Destroy all children
        for (VObject* child : this->children) {
            child->destroy();
        }

        this->scene->delete_vobject(this);
    }

    Transform& VObject::get_transform() {
        return this->transform;
    }
}
