#include <engine/vobject/Transform.hpp>
#include <engine/vobject/components/Drawable.hpp>
#include <engine/vobject/components/Camera.hpp>
#include <engine/EngineController.hpp>
#include <engine/vao.hpp>
#include <engine/math/matrices.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particles : public engine::Drawable {
    public:
        Particles(int max_particles, float* speed);
        ~Particles();

        void update();
        void draw() override;

    private:
        struct Particle {
            engine::Vec3 position;
            float radial_distance;
            float theta;
        };

        std::vector<Particle> particles;
        int max_particles;
        float max_radial_distance;
        float max_distance;
        float* speed;
        bool use_phong_shading = false;

        engine::Vec3 diffuse_reflectance = engine::Vec3(1.0f, 1.0f, 1.0f);
        engine::Vec3 specular_reflectance = engine::Vec3(1.0f, 1.0f, 1.0f);
        engine::Vec3 environment_reflectance = engine::Vec3(1.0f, 1.0f, 1.0f);
        float specular_exponent = 32.0f;
};