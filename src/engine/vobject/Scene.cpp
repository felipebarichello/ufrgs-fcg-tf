#include "Scene.hpp"

namespace engine {
    VObject* Scene::new_vobject() {
        auto key = this->next_vobject_id++;
        auto result = this->vobjects.emplace(key, VObject(this, key));
        auto vobject = &result.first->second;
        vobject->awaken();
        return vobject;
    }

    void Scene::delete_vobject(VObject* vobject) {
        auto key = vobject->get_id();
        this->vobjects.erase(key);
    }
} // namespace engine
