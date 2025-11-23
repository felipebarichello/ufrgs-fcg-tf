#include "EnemyShip.hpp"
#include "SpaceshipObj.hpp"
#include <game/components/include.hpp>

using namespace engine;
using namespace game::components;

namespace game::instantiators {
    VObjectConfig EnemyShip() {
        KinematicBody* kinematic = new KinematicBody();
        AngularVelocity* angular = new AngularVelocity();
        SpaceshipController* ship_controller = new SpaceshipController(kinematic, angular);
        CylinderCollider* cylinder_collider = new CylinderCollider(2.0f, 1.0f);
        EnemyShipController* enemy_controller = new EnemyShipController(ship_controller, cylinder_collider);

        ship_controller->fuel = std::numeric_limits<float>::infinity();

        return VObjectConfig()
            .transform(TransformBuilder()
                .position(Vec3(0.0f, 400.0f, 0.0f)))
            .component(cylinder_collider)
            .component(enemy_controller)
            .component(ship_controller)
            .component(kinematic)
            .component(angular)
            .child(SpaceshipObj());
    }
} // namespace game::instantiators