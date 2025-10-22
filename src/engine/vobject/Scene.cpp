#include "Scene.hpp"
#include <memory>

namespace engine {
    VObject* Scene::instantiate(VObjectConfig& vobject_config) {
        VObject* vobject = this->new_vobject(vobject_config._transform);

        // Add components to the VObject
        for (auto& component : vobject_config.components) {
            auto component_unique = std::unique_ptr<Component>(component);
            vobject->add_component(std::move(component_unique));
        }

        // Recursively instantiate and add children VObjects
        for (VObjectConfig& child_config : vobject_config.children) {
            VObject* child_vobject = this->instantiate(child_config);
            vobject->add_child(child_vobject);
        }

        return vobject;
    }

    VObject* Scene::new_vobject(Transform transform) {
        auto key = this->next_vobject_id++;
        auto result = this->vobjects.emplace(key, std::make_unique<VObject>(this, key));
        auto vobject = result.first->second.get();

        transform.vobject_ptr = vobject;
        vobject->_transform = transform;

        return vobject;
    }

    void Scene::delete_vobject(VObject* vobject) {
        auto key = vobject->get_id();
        this->vobjects.erase(key);
    }
} // namespace engine
