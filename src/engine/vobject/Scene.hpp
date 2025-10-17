#pragma once

#include <vector>
#include "VObject.hpp"
#include <unordered_map>

namespace engine {
    class Scene {
        friend class VObject;

        public:
            // TODO: should this constructor be public?
            Scene() = default;

            // TODO: should this be public?
            VObject* create_vobject();

        private:
            /// @brief 0 can be used as an invalid ID.
            VObjectId next_vobject_id = 1;
            std::unordered_map<VObjectId, VObject> vobjects;
            
            void remove_vobject(VObject* vobject);
    };
}
