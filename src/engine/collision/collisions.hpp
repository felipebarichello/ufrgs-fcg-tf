#pragma once

#include <engine/collision/colliders/SphereCollider.hpp>

#include <engine/collision/colliders/PointCollider.hpp>
#include <engine/collision/colliders/CapsuleCollider.hpp>

namespace engine::collision {
    class PointSphereCollision {
        public:
            PointSphereCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    PointSphereCollision collide_point_sphere(const PointCollider& point, const SphereCollider& sphere);

    class CapsuleCapsuleCollision {
        public:
            CapsuleCapsuleCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    class CapsuleSphereCollision {
        public:
            CapsuleSphereCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    CapsuleSphereCollision collide_capsule_sphere(const CapsuleCollider& cyl, const SphereCollider& sphere);

    class PointCapsuleCollision {
        public:
            PointCapsuleCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    PointCapsuleCollision collide_point_capsule(const PointCollider& point, const CapsuleCollider& capsule);
}
