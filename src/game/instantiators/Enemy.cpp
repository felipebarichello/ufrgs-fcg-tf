#include "Enemy.hpp"
#include <vector>

#include <game/scenes/MainScene_vars.hpp>

using namespace engine;
using namespace game::components;

namespace game::instantiators {
    
    VObjectConfig Enemy(EnemyConfig config) {
        PointCollider* point_collider = new PointCollider();
        CapsuleCollider* capsule_collider = new CapsuleCollider(9.0f, 0.5f);
        KinematicBody* kinematic = new KinematicBody();
        Gravity* gravity = new Gravity(kinematic);
        WalkerController* walker = new WalkerController(kinematic, gravity, point_collider);

        walker->set_max_walk_speed(40.0f);
        
        return VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, config.home->get_radius(), 0.0f)))
            .component(walker)
            .component(point_collider)
            .component(new GroundEnemyController(walker, capsule_collider))
            .component(gravity)
            .component(kinematic)
            .child(VObjectConfig()
                .transform(TransformBuilder()
                    .rotation(Quaternion::from_z_rotation(-M_PI/2)))
                .component(capsule_collider)
            )
            .child(EnemyObj());
    }
    
    VObjectConfig EnemyObj() {
        return VObjectConfig()
            .transform(TransformBuilder()
                .scale(0.002f)
                .position(Vec3(0.0f, 2.0f, 0.0f))
            )
            .component(new ObjDrawable(std::string("10477_Satellite_v1_L3.obj"), std::string("10477_Satellite_v1_Diffuse.jpg")));
    }

}
