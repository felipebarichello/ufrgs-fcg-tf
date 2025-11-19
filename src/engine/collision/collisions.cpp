#include <engine>

namespace engine::collision {
    SphereSphereCollision collide_sphere_sphere(const SphereCollider& sphere_1, const SphereCollider& sphere_2) {
        // Get world positions from attached VObjects (if any).
        auto vobj1 = sphere_1.get_vobject();
        auto vobj2 = sphere_2.get_vobject();

        Vec3 pos1 = vobj1->transform().get_position();
        Vec3 pos2 = vobj2->transform().get_position();

        // Vector between centers
        Vec3 delta = pos2 - pos1;

        // Compare squared distance to squared sum of radii to avoid sqrt
        float dist2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        float rsum = sphere_1.get_radius_squared() + sphere_2.get_radius_squared();
        bool collided = dist2 <= (rsum * rsum);

        return SphereSphereCollision(collided);
    }

    PointSphereCollision collide_point_sphere(const PointCollider& point, const SphereCollider& sphere) {
        auto point_vobject = point.get_vobject();
        auto sphere_vobject = sphere.get_vobject();

        Vec3 point_pos = point_vobject->transform().get_position();
        Vec3 sphere_pos = sphere_vobject->transform().get_position();

        engine::Vec3 delta = point_pos - sphere_pos;
        float distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        bool collided = distance_squared <= sphere.get_radius_squared();

        return PointSphereCollision(collided);
    }

    CylinderCylinderCollision collide_cylinder_cylinder(const CylinderCollider& c1, const CylinderCollider& c2) {
        // Get world positions from attached VObjects (if any).
        auto vobj1 = c1.get_vobject();
        auto vobj2 = c2.get_vobject();

        engine::Vec3 pos1 {0.0f, 0.0f, 0.0f};
        engine::Vec3 pos2 {0.0f, 0.0f, 0.0f};

        if (vobj1) pos1 = vobj1->transform().get_position();
        if (vobj2) pos2 = vobj2->transform().get_position();

        // Horizontal distance on XZ plane
        float dx = pos2.x - pos1.x;
        float dz = pos2.z - pos1.z;
        float horiz_dist2 = dx*dx + dz*dz;
        float rsum = c1.get_radius() + c2.get_radius();

        bool horiz_overlap = horiz_dist2 <= (rsum * rsum);

        // Vertical overlap along Y axis: compare center Y distance to half-heights
        float half_h1 = c1.get_height() * 0.5f;
        float half_h2 = c2.get_height() * 0.5f;
        float dy = std::fabs(pos2.y - pos1.y);
        bool vert_overlap = dy <= (half_h1 + half_h2);

        return CylinderCylinderCollision(horiz_overlap && vert_overlap);
    }
} // namespace engine::collision
