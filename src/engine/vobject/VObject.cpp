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

    void VObject::disown_child(VObject* child) {
        auto obj_id = child->get_id();
        std::size_t elements_destroyed = static_cast<bool>(this->children.erase(obj_id));

        // TODO: Better error logging
        if (elements_destroyed <= 0) {
            std::cerr << "VObject " << this->id
                << " tried to disown child " << child->get_id();
                
            auto real_parent = child->get_parent();
            if (real_parent) {
                std::cerr << " that belongs to " << real_parent.value();
            } else {
                std::cerr << " that does not have a parent";
            }
            
            return;
        }

        child->set_parent_raw(std::nullopt);
    }

    void VObject::destroy() {
        // Tell the components they are being destroyed so they can clean up
        for (auto& item : this->components) {
            item->PreDestroy();
        }

        // Destroy all children
        for (auto child : this->children) {
            child.second->destroy();
        }

        this->scene->delete_vobject(this);
    }

    Transform& VObject::transform() {
        return this->_transform;
    }
}
