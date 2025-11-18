#include <engine>

namespace engine::collision {
    SphereSphereCollision collide_sphere_sphere(SphereCollider& sphere_1, SphereCollider& sphere_2) {
        // Get world positions from attached VObjects (if any).
        auto vobj1 = sphere_1.get_vobject();
        auto vobj2 = sphere_2.get_vobject();

        Vec3 pos1 {0.0f, 0.0f, 0.0f};
        Vec3 pos2 {0.0f, 0.0f, 0.0f};

        pos1 = vobj1->transform().get_position();
        pos2 = vobj2->transform().get_position();

        // Vector between centers
        Vec3 delta = pos2 - pos1;

        // Compare squared distance to squared sum of radii to avoid sqrt
        float dist2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        float rsum = sphere_1.get_radius() + sphere_2.get_radius();
        bool collided = dist2 <= (rsum * rsum);

        return SphereSphereCollision(collided);
    }
} // namespace engine::collision
