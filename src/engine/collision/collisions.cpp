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

    // Squared distance between two segments [p1,q1] and [p2,q2]
    static float segment_segment_distance_squared(const engine::Vec3& p1, const engine::Vec3& q1, const engine::Vec3& p2, const engine::Vec3& q2) {
        engine::Vec3   d1 = q1 - p1; // Direction vector of segment S1
        engine::Vec3   d2 = q2 - p2; // Direction vector of segment S2
        engine::Vec3   r = p1 - p2;
        float a = engine::dot(d1,d1); // squared length of segment S1
        float e = engine::dot(d2,d2); // squared length of segment S2
        float f = engine::dot(d2,r);

        float s, t;

        const float EPS = 1e-12f;

        if (a <= EPS && e <= EPS) {
            // Both segments degenerate to points
            return engine::dot(p1 - p2, p1 - p2);
        }
        if (a <= EPS) {
            // First segment degenerates to a point
            s = 0.0f;
            t = clampf(f / e, 0.0f, 1.0f);
        } else {
            float c = engine::dot(d1, r);
            if (e <= EPS) {
                // Second segment degenerates to a point
                t = 0.0f;
                s = clampf(-c / a, 0.0f, 1.0f);
            } else {
                float b = engine::dot(d1, d2);
                float denom = a*e - b*b;
                if (denom != 0.0f) {
                    s = clampf((b*f - c*e) / denom, 0.0f, 1.0f);
                } else {
                    s = 0.0f;
                }
                t = (b*s + f) / e;

                if (t < 0.0f) {
                    t = 0.0f;
                    s = clampf(-c / a, 0.0f, 1.0f);
                } else if (t > 1.0f) {
                    t = 1.0f;
                    s = clampf((b - c) / a, 0.0f, 1.0f);
                }
            }
        }

        engine::Vec3 c1 = p1 + d1 * s;
        engine::Vec3 c2 = p2 + d2 * t;
        engine::Vec3 diff = c1 - c2;
        return engine::dot(diff, diff);
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

    CylinderCylinderCollision collide_cylinder_cylinder(const CylinderCollider& c1, const CylinderCollider& c2) {
        // Get world positions and transforms
        auto vobj1 = c1.get_vobject();
        auto vobj2 = c2.get_vobject();

        Vec3 pos1 = vobj1->transform().get_world_position();
        Vec3 pos2 = vobj2->transform().get_world_position();

        // Axes (apply rotation)
        Vec3 axis1 = vobj1->transform().get_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
        Vec3 axis2 = vobj2->transform().get_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));

        // Segment endpoints for the axes
        Vec3 a1 = pos1;
        Vec3 b1 = pos1 + axis1 * c1.get_height();
        Vec3 a2 = pos2;
        Vec3 b2 = pos2 + axis2 * c2.get_height();

        float dist2 = segment_segment_distance_squared(a1, b1, a2, b2);
        float rsum = c1.get_radius() + c2.get_radius();
        bool collided = dist2 <= (rsum * rsum);

        return CylinderCylinderCollision(collided);
    }

    CylinderSphereCollision collide_cylinder_sphere(const CylinderCollider& cyl, const SphereCollider& sphere) {
        auto cylinder_vobj = cyl.get_vobject();
        auto sphere_vobj = sphere.get_vobject();

        Vec3 cyl_pos = cylinder_vobj->transform().get_world_position();
        Vec3 sphere_pos = sphere_vobj->transform().get_world_position();

        // Cylinder axis and scale
        Vec3 axis = cylinder_vobj->transform().get_quaternion().rotate(Vec3(0.0f, 1.0f, 0.0f));
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

} // namespace engine::collision
