#include "Gravity.hpp"
#include <engine/math/linalg.hpp>
#include <engine/EngineController.hpp>

using namespace engine;

namespace game::components {
    
    void Gravity::Update() {
        Vec3 equivalent_gravity = this->compute_equivalent_gravity();
        this->kinematic->set_velocity(this->kinematic->get_velocity() + equivalent_gravity * EngineController::get_delta_time());
    }

    Vec3 Gravity::compute_equivalent_gravity() {
        Vec3 gravity_sum(0.0f);
        Transform& transform = this->get_vobject()->transform();
        for (PlanetInfo* planet : this->planets) {
            Vec3 vec_to_planet = planet->get_vobject()->transform().get_world_position() - transform.get_world_position();
            float distance_to_planet = std::max(engine::h_norm(vec_to_planet), 1e-6f);
            Vec3 grav_direction = engine::h_normalize(vec_to_planet);
            gravity_sum += (planet->get_gravity_mass() / distance_to_planet) * grav_direction;
        }

        return gravity_sum * Gravity::GRAVITATIONAL_CONSTANT;
    }

} // namespace game::components
