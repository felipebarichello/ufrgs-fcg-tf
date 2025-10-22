#pragma once

#include <vector>
#include "VObject.hpp"
#include <unordered_map>
#include "SceneScheduler.hpp"
#include "VObjectConfig.hpp"

namespace engine {
    // CRITICAL: This class manages lifetimes of many connected things.
    // Be careful when modifying it.
    // Use after free and dangling pointer hazard.
    // Think a lot before changing anything here.
    class Scene {
        friend class VObject;

        public:
            Scene() = default;
            VObject* instantiate(VObjectConfig& vobject_config);

        private:
            /// @brief 0 can be used as an invalid ID.
            VObject::Id next_vobject_id = 1;
            std::unordered_map<VObject::Id, std::unique_ptr<VObject>> vobjects;
            SceneScheduler scheduler;
            
            VObject* new_vobject(Transform transform);
            void delete_vobject(VObject* vobject);
    };
}
