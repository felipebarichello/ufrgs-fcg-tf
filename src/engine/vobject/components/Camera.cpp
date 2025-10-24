#include <engine>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/math/linalg.hpp>
#include <macros>

using engine::EngineController;

namespace engine {
    Mat4 Camera::get_perspective_matrix() const {
        float n = this->near_distance;
        float f = this->far_distance;

        float t = -n * tanf(this->fov / 2.0f);
        float b = -t;
        float r = t * EngineController::get_instance()->get_screen_ratio();
        float l = -r;

        Mat4 p_matrix = h_line_matrix(
            n,    0.0f, 0.0f, 0.0f,
            0.0f, n,    0.0f, 0.0f,
            0.0f, 0.0f, n+f,  -f*n,
            0.0f, 0.0f, 1.0f, 0.0f
        );

        Mat4 m_matrix = h_ortographic_matrix(l, r, b, t, n, f);

        return -m_matrix * p_matrix;
    }
}
