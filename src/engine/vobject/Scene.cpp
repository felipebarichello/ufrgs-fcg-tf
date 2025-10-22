#include "Scene.hpp"
#include <memory>

namespace engine {
    VObject* Scene::instantiate(VObjectConfig& vobject_config) {
        VObject* vobject = this->new_vobject();
        
        // Add components to the VObject
        for (auto& component : vobject_config.components) {
            auto c = std::unique_ptr<Component>(component);
            vobject->add_component(std::move(c));
        }

        // Recursively instantiate and add children VObjects
        for (VObjectConfig& child_config : vobject_config.children) {
            VObject* child_vobject = this->instantiate(child_config);
            vobject->add_child(child_vobject);
        }

        return vobject;
    }

    VObject* Scene::new_vobject() {
        auto key = this->next_vobject_id++;
        auto result = this->vobjects.emplace(key, VObject(this, key));
        auto vobject = &result.first->second;
        return vobject;
    }

    void Scene::delete_vobject(VObject* vobject) {
        auto key = vobject->get_id();
        this->vobjects.erase(key);
    }
} // namespace engine
