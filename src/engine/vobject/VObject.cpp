#include "VObject.hpp"
#include "Scene.hpp"

namespace engine {
    void VObject::destroy() {
        for (VObject* child : this->children) {
            child->destroy();
        }

        this->scene->remove_vobject(this);
    }
}
