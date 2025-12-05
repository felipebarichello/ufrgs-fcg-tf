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

    CapsuleSphereCollision collide_capsule_sphere(const CapsuleCollider& cyl, const SphereCollider& sphere) {
        auto capsule_vobj = cyl.get_vobject();
        auto sphere_vobj = sphere.get_vobject();

        Vec3 cyl_pos = capsule_vobj->transform().get_world_position();
        Vec3 sphere_pos = sphere_vobj->transform().get_world_position();

        // Capsule axis and scale
        Vec3 axis = capsule_vobj->transform().get_world_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        float cyl_radius = cyl.get_radius();

        float sphere_radius = sphere.get_radius();

        // Capsule segment endpoints
        Vec3 a = cyl_pos + axis * cyl.get_height()*0.5f;
        Vec3 b = cyl_pos - axis * cyl.get_height()*0.5f;

        // Closest point on capsule segment to sphere center
        Vec3 closest = closest_point_on_segment(a, b, sphere_pos);
        Vec3 diff = sphere_pos - closest;
        float sqr_dist = engine::dot(diff, diff);

        bool collided = sqr_dist <= ((cyl_radius + sphere_radius) * (cyl_radius + sphere_radius));
        return CapsuleSphereCollision(collided);
    }

    PointCapsuleCollision collide_point_capsule(const PointCollider& point, const CapsuleCollider& cyl) {
        auto point_vobj = point.get_vobject();
        auto capsule_vobj = cyl.get_vobject();

        Vec3 point_pos = point_vobj->transform().get_world_position();
        Vec3 cyl_pos = capsule_vobj->transform().get_world_position();

        // Capsule axis and scale
        Vec3 axis = capsule_vobj->transform().get_world_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        float cyl_radius = cyl.get_radius();

        // Capsule segment endpoints
        Vec3 a = cyl_pos + axis * cyl.get_height()*0.5f;
        Vec3 b = cyl_pos - axis * cyl.get_height()*0.5f;

        // Closest point on capsule segment to point
        Vec3 closest = closest_point_on_segment(a, b, point_pos);
        Vec3 diff = point_pos - closest;
        float sqr_dist = engine::dot(diff, diff);

        bool collided = sqr_dist <= (cyl_radius * cyl_radius);
        return PointCapsuleCollision(collided);
    }

} // namespace engine::collision
