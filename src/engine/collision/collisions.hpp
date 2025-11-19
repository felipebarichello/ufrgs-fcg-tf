#pragma once

#include <engine/collision/colliders/SphereCollider.hpp>

#include <engine/collision/colliders/PointCollider.hpp>
#include <engine/collision/colliders/CylinderCollider.hpp>

namespace engine::collision {
    class SphereSphereCollision {
        public:
            SphereSphereCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    SphereSphereCollision collide_sphere_sphere(const SphereCollider& sphere_1, const SphereCollider& sphere_2);

    class PointSphereCollision {
        public:
            PointSphereCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    PointSphereCollision collide_point_sphere(const PointCollider& point, const SphereCollider& sphere);

    class CylinderCylinderCollision {
        public:
            CylinderCylinderCollision(bool collided) : collided(collided) {}
            bool has_collided() const { return this->collided; }

        private:
            bool collided;
    };

    CylinderCylinderCollision collide_cylinder_cylinder(const CylinderCollider& c1, const CylinderCollider& c2);
}
