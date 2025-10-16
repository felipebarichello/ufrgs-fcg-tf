#include "camera.hpp"

Camera::Camera(InputController input_controller) {
    this->phi = 0.0f;
    this->theta = 3.141592f; // Looking towards -Z
    input_controller.subscribe_hold_button(GLFW_MOUSE_BUTTON_LEFT, &this->mouse_button_pressed);
}

void Camera::update_free_camera_direction() {

    if (!this->mouse_button_pressed)
        return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    glm::vec2 cursor_delta = g_engine_controller.input()->get_cursor_position_delta();

    // Atualizamos parâmetros da câmera com os deslocamentos
    this->theta -= 0.01f*cursor_delta.x;
    this->phi   += 0.01f*cursor_delta.y;


    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    g_free_camera_view_unit_vector = Vec3(
        cosf(g_CameraPhi) * sinf(g_CameraTheta),
        sinf(g_CameraPhi),
        cosf(g_CameraPhi) * cosf(g_CameraTheta)
    );
}

void Camera::update() {

    Vec3 camera_position_c; // Camera center (position)
    Vec3 camera_view_unit_vector; // Direction the camera is pointing

    if (g_camera_is_free) {
        // Update da posição da câmera de acordo com o input de movimento
        update_free_camera_position();
        update_free_camera_direction();

        camera_position_c = g_free_camera_position;
        camera_view_unit_vector = g_free_camera_view_unit_vector;

        float y = sin(g_CameraPhi);
        float z = cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = cos(g_CameraPhi)*sin(g_CameraTheta);
        
        camera_view_unit_vector = Vec3(x, y, z);
    } else {
        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        camera_position_c = Vec3(x, y, z);
        Vec3 camera_lookat_point = Vec3(0.0f, 0.0f, 0.0f);
        camera_view_unit_vector = glm::normalize(camera_lookat_point - camera_position_c); 
    }

    CameraTransform cam_transform = g_camera.cam_transform();
    cam_transform.set_position(camera_position_c);
    cam_transform.set_basis_from_up_view(g_free_camera_up_vector, camera_view_unit_vector);
    glm::mat4 view = invert_orthonormal_matrix(cam_transform.get_matrix());
    g_free_camera_right_vector = Vec3(view[0][0], view[1][0], view[2][0]);

    glm::mat4 projection;

    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = 3.141592f / 3.0f;
    projection = Matrix_Perspective(field_of_view, g_engine_controller.get_screen_ratio(), nearplane, farplane);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

}