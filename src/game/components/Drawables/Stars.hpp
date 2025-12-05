#include <engine>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <game/components/player/PlayerShipController.hpp>
#include <engine/vobject/components/Camera.hpp>

class Stars : public engine::Drawable {
    public:
        Stars(int max_stars);
        ~Stars();

        void update();
        void draw() override;

    private:
        struct Star {
            engine::Vec3 position;
            float phi;
            float theta;
            float alpha;
            float size;
        };

        std::vector<Star> stars;
        int max_stars;
        float distance = 200.0f;
        bool use_phong_shading = false;

        float max_size = 3.0f;
        float min_size = 0.5f;

        float min_alpha = 0.25f;
        float max_alpha = 1.0f;

        engine::Vec3 color = {1.0, 1.0, 1.0};};