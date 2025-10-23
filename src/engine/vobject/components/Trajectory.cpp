#include "Trajectory.hpp"

using namespace engine;

Trajectory::Trajectory(const Curve& curve, float speed) {
    this->curve = curve;
    this->t = 0.0f;
    this->speed = speed;
}

void Trajectory::Awake() {
    // Initialize the trajectory    
}

void Trajectory::Start() {
    // Start the trajectory
}

void Trajectory::Update() {
    auto& transform = this->get_vobject()->transform();
    float delta_time = EngineController::get_delta_time();
    Vec3 tangent = curve.get_tangent(this->t);
    this->t += this->speed * delta_time / norm(tangent);
    if (this->t > 1.0f) {
        this->t -= 1.0f;
    }
    transform.position() = curve.get_point(this->t);
}