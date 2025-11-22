#include "Enemy.hpp"
#include <vector>

using namespace engine;
using namespace game::components;

namespace game::instantiators {

    VObjectConfig Enemy(EnemyConfig config) {
        PointCollider* point_collider = new PointCollider();
        CylinderCollider* cylinder_collider = new CylinderCollider(1.0f, 3.0f);
        KinematicBody* kinematic = new KinematicBody();
        Gravity* gravity = new Gravity(kinematic, config.planets);
        WalkerController* walker = new WalkerController(kinematic, gravity, config.planets, point_collider);

        return VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, config.home->get_radius(), 0.0f)))
            .component(walker)
            .component(point_collider)
            .component(cylinder_collider)
            .component(new GroundEnemyController(walker, cylinder_collider, config.player))
            .component(gravity)
            .component(kinematic)
            .child(EnemyObj());
    }
    
    VObjectConfig EnemyObj() {
        return VObjectConfig()
            .transform(TransformBuilder()
                .scale(0.03f)
                .position(Vec3(0.0f, -2.0f, 0.0f))
                .rotation(Quaternion::from_y_rotation(3.141592f * 1.5f))
            )
            .component(new ObjDrawable(std::string("VacuumCleaner/VacuumCleaner.obj"), std::string("spaceship.jpg")));
    }

}
