#include "Scene.hpp"

namespace engine {
    VObject* Scene::create_vobject() {
        auto key = this->next_vobject_id++;
        auto result = this->vobjects.emplace(key, VObject(this, key));
        auto vobject = &result.first->second;

        vobject->awake();

        return vobject;
    }

    void Scene::remove_vobject(VObject* vobject) {
        auto key = vobject->get_id();
        this->vobjects.erase(key);
    }
} // namespace engine