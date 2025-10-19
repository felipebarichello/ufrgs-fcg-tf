#include <GL/glew.h>

namespace engine {
    class drawable {
    public:
        virtual void draw(GLint model_uniform) = 0;
    };
}