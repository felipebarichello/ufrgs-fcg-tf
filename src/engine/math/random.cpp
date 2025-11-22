#include "random.hpp"
#include <random>

namespace engine {
    float random_float(float min_inclusive, float max_inclusive) {
        static thread_local std::mt19937 rng((std::random_device())());
        std::uniform_real_distribution<float> dist(min_inclusive, max_inclusive);
        return dist(rng);
    }
}
