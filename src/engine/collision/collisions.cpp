#include <engine>
#include <algorithm>
#include <cmath>

namespace engine::collision {

    static float clampf(float v, float lo, float hi) {
        return std::max(lo, std::min(hi, v));
    }

    // Closest point on segment AB to point P
    static engine::Vec3 closest_point_on_segment(const engine::Vec3& a, const engine::Vec3& b, const engine::Vec3& p) {
        engine::Vec3 ab = b - a;
        float ab2 = engine::dot(ab, ab);
        if (ab2 <= 1e-12f) return a;
        float t = engine::dot(p - a, ab) / ab2;
        t = clampf(t, 0.0f, 1.0f);
        return a + ab * t;
    }

    SphereSphereCollision collide_sphere_sphere(const SphereCollider& sphere_1, const SphereCollider& sphere_2) {
        // Get world positions from attached VObjects (if any).
        auto vobj1 = sphere_1.get_vobject();
        auto vobj2 = sphere_2.get_vobject();

        Vec3 pos1 = vobj1->transform().get_world_position();
        Vec3 pos2 = vobj2->transform().get_world_position();

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

        Vec3 point_pos = point_vobject->transform().get_world_position();
        Vec3 sphere_pos = sphere_vobject->transform().get_world_position();

        Vec3 delta = point_pos - sphere_pos;
        float distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        bool collided = distance_squared <= sphere.get_radius_squared();

        return PointSphereCollision(collided);
    }

    CylinderSphereCollision collide_cylinder_sphere(const CylinderCollider& cyl, const SphereCollider& sphere) {
        auto cylinder_vobj = cyl.get_vobject();
        auto sphere_vobj = sphere.get_vobject();

        Vec3 cyl_pos = cylinder_vobj->transform().get_world_position();
        Vec3 sphere_pos = sphere_vobj->transform().get_world_position();

        // Cylinder axis and scale
        Vec3 axis = cylinder_vobj->transform().get_local_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        float cyl_radius = cyl.get_radius();

        float sphere_radius = sphere.get_radius();

        // Cylinder segment endpoints
        Vec3 a = cyl_pos;
        Vec3 b = cyl_pos + axis * cyl.get_height();

        // Closest point on cylinder segment to sphere center
        Vec3 closest = closest_point_on_segment(a, b, sphere_pos);
        Vec3 diff = sphere_pos - closest;
        float dist2 = engine::dot(diff, diff);

        bool collided = dist2 <= ((cyl_radius + sphere_radius) * (cyl_radius + sphere_radius));
        return CylinderSphereCollision(collided);
    }

    PointCylinderCollision collide_point_cylinder(const PointCollider& point, const CylinderCollider& cyl) {
        auto point_vobj = point.get_vobject();
        auto cylinder_vobj = cyl.get_vobject();

        Vec3 point_pos = point_vobj->transform().get_world_position();
        Vec3 cyl_pos = cylinder_vobj->transform().get_world_position();

        // Cylinder axis and scale
        Vec3 axis = cylinder_vobj->transform().get_local_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        float cyl_radius = cyl.get_radius();

        // Cylinder segment endpoints
        Vec3 a = cyl_pos + axis * cyl.get_height()*0.5f;
        Vec3 b = cyl_pos - axis * cyl.get_height()*0.5f;

        // Closest point on cylinder segment to point
        Vec3 closest = closest_point_on_segment(a, b, point_pos);
        Vec3 diff = point_pos - closest;
        float dist2 = engine::dot(diff, diff);

        bool collided = dist2 <= (cyl_radius * cyl_radius);
        return PointCylinderCollision(collided);
    }

} // namespace engine::collision
