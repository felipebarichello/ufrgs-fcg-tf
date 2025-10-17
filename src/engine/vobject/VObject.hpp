#pragma once

#include <vector>

namespace engine {
    class Scene; // Forward declaration

    using VObjectId = uint64_t;

    class VObject {
        public:
            VObject(Scene* scene, VObjectId id)
                : scene(scene), id(id) {}

            VObjectId get_id() const {
                return this->id;
            }

            void destroy();

        private:
            Scene* scene;
            VObjectId id;
            std::vector<VObject*> children;
    };
}
