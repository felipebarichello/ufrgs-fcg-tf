#pragma once

#include <vector>
#include "VObject.hpp"
#include <unordered_map>

namespace engine {
    class Scene {
        friend class VObject;

        public:
            Scene() = default;

        private:
            /// @brief 0 can be used as an invalid ID.
            VObject::Id next_vobject_id = 1;
            std::unordered_map<VObject::Id, VObject> vobjects;
            
            VObject* new_vobject();
            void delete_vobject(VObject* vobject);
    };
}
