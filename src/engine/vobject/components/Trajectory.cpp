#include "Trajectory.hpp"

using namespace engine;

Trajectory::Trajectory(std::unique_ptr<Curve> curve, float speed) {
    this->curve = std::move(curve);
    this->t = 0.0f;
    this->speed = speed;
}

Trajectory::Trajectory(std::unique_ptr<Curve> curve, float speed, float t) {
    this->curve = std::move(curve);
    this->t = t;
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
    Vec3 tangent = curve->get_tangent(this->t);
    this->t += this->speed * delta_time / h_norm(tangent);
    if (this->t > 1.0f) {
        this->t -= 1.0f;
    }
    transform.local_position() = curve->get_point(this->t);
}