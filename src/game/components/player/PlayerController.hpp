#pragma once

#include <engine>
#include "HumanoidPlayerController.hpp"
#include <game/components/player/PlayerShipController.hpp>

namespace game::components {
    class PlayerController : public engine::Behavior {
    public:
        PlayerController(HumanoidPlayerController* humanoid, PlayerShipController* spaceship, engine::Camera* humanoid_cam, engine::Camera* ship_cam)
            : humanoid(humanoid), ship(spaceship), humanoid_cam(humanoid_cam), ship_cam(ship_cam) {}

        void Start() override;
        void Update() override;

        HumanoidPlayerController* get_humanoid() { return this->humanoid; }
        PlayerShipController* get_ship() { return this->ship; }

        // Called by enemies when they hit this player
        void hit_by_enemy();

    private:
        HumanoidPlayerController* humanoid = nullptr;
        PlayerShipController* ship = nullptr;
        engine::Camera* humanoid_cam = nullptr;
        engine::Camera* ship_cam = nullptr;

        void toggle_mode();
    };
}
