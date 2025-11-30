#pragma once

#include <engine>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <game/components/SpaceshipController.hpp>
#include <game/components/player/ShipCameraController.hpp>


namespace game::components {
    class ThrustParticles : public engine::Drawable {
        public:
            ThrustParticles(SpaceshipController* ship_controller);
            ~ThrustParticles();

            void update();
            void draw() override;

        private:
            struct Particle {
                engine::Vec3 position;
                engine::Vec3 velocity;
                float decay_time;       // life duration (seconds)
                float size;
                float age;      // current life progress (seconds)
            };

            SpaceshipController* ship_controller = nullptr;
            std::vector<Particle> particles;

            engine::Vec3 thruster_offset = engine::Vec3(0.0f, 0.0f, 2.0f); // Offset in thruster local coords from center of thruster Transform to get thruster position
            engine::Vec3 thruster_normal = engine::Vec3(0.0f, 0.0f, 1.0f); // Direction the thruster points to in thruster local coords
            float particles_per_second = 50.0f;
            float thruster_radius = 1.0f; // radius of emission circle behind ship
            float min_particle_decay_rate = 0.25f;   // seconds
            float max_particle_decay_rate = 2.0f;    // seconds
            float max_particle_size = 2.0f;
            engine::Vec3 initial_color = {0.267f, 0.278f, 0.647f}; // #4447a5ff
            engine::Vec3 final_color  = {0.957f, 0.694f, 0.071f};  // #f4b112ff
            // engine::Vec3 final_color   = {0.973f, 0.282f, 0.063f}; // #f84810
            
            engine::Transform* thruster_transform() {
                return &this->ship_controller->get_vobject()->transform();
            }
    };  
} // namespace game::components
