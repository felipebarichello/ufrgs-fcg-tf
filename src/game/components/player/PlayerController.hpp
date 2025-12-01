#pragma once

#include <engine>
#include "HumanoidPlayerController.hpp"
#include "PlayerShipController.hpp"

namespace game::components {
    class PlayerController : public engine::Behavior {
    public:
        PlayerController(HumanoidPlayerController* humanoid, PlayerShipController* spaceship, engine::Camera* humanoid_cam, engine::Camera* ship_cam)
            : humanoid(humanoid), ship(spaceship), humanoid_cam(humanoid_cam), ship_cam(ship_cam) {}

        void Awake() override;
        void Start() override;
        void Update() override;
        void PostUpdate() override;

        HumanoidPlayerController* get_humanoid() { return this->humanoid; }
        PlayerShipController* get_ship() { return this->ship; }

        // Called by enemies when they hit this player
        void hit_by_enemy();

        void restart();

    private:
        HumanoidPlayerController* humanoid = nullptr;
        PlayerShipController* ship = nullptr;
        engine::Camera* humanoid_cam = nullptr;
        engine::Camera* ship_cam = nullptr;

        engine::TextDrawable* game_over_text = nullptr;
        engine::TextDrawable* fuel_text = nullptr;
        engine::TextDrawable* oxygen_text = nullptr;
        engine::TextDrawable* time_text = nullptr;

        float oxygen_level = 100.0f; // percentage
        float oxygen_decrease_rate = 5.0f; // percentage per seconds  
        float fuel_increase_rate = 5.0f; // units per second when grounded
        bool killed_by_enemy = false;
        bool game_overed = false;
        float game_over_timer = 0.0f;
        float time_to_restart = 3.0f; // seconds before restart after game over
        float time = 0.0f;

        void game_over_routine();

        void toggle_mode();

        void game_over(std::string reason);
    };
}
