#include <engine/math/curves/Curve.hpp>
#include <engine/vobject/VObject.hpp>
#include <engine/vobject/Transform.hpp>
#include <engine/vobject/Behavior.hpp>
#include <engine/EngineController.hpp>
#include <memory>

namespace engine {
    class Trajectory : public Behavior {
        public:
            Trajectory() = default;
            Trajectory(std::unique_ptr<Curve> curve, float speed);
            Trajectory(std::unique_ptr<Curve> curve, float speed, float t);
            void Awake() override;
            void Start() override;
            void Update() override;
        private:
            std::unique_ptr<Curve> curve;
            float t;
            float speed = 0.1f; // Units per second
    };
}
