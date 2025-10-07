 #include "globals.hpp"
 
int main() {
    WindowConfig window_config = WindowConfig(
        800,
        800,
        "INF01047 - 579876 - Felipe Wendt Barichello"
    );

    g_engine_controller = EngineController::start_engine(window_config);
    
    InputController& input_controller = g_engine_controller.input();
    input_controller.attach_key_handler(KeyCallback);
    input_controller.attach_mouse_button_handler(MouseButtonCallback);
    input_controller.attach_cursor_position_handler(CursorPosCallback);
    input_controller.attach_scrolling_handler(ScrollCallback);

    g_window = g_engine_controller.get_window();

    cube_faces_vao = BuildCubeFaces();
    cube_edges_vao = BuildCubeEdges();
    cube_axes_vao  = BuildCubeAxes();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    g_model_uniform      = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_engine_controller.get_gpu_program_id(), "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    g_engine_controller.hand_over_control(update);
    return 0;
}

void update() {
    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(g_engine_controller.get_gpu_program_id());

    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTriangles(). Veja
    // comentários detalhados dentro da definição de BuildTriangles().
    glBindVertexArray(g_vertex_array_object_id);

    Vec3 camera_position_c; // Camera center (position)
    Vec3 camera_view_unit_vector; // Direction the camera is pointing

    if (g_camera_is_free) {
        // Update da posição da câmera de acordo com o input de movimento
        update_free_camera_position();

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

    // Agora computamos a matriz de Projeção.
    glm::mat4 projection;

    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -10.0f; // Posição do "far plane"

    if (g_UsePerspectiveProjection) {
        // Projeção Perspectiva.
        // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
        float field_of_view = 3.141592f / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_engine_controller.get_screen_ratio(), nearplane, farplane);
    } else {
        // Projeção Ortográfica.
        // Para definição dos valores l, r, b, t ("left", "right", "bottom", "top"),
        // PARA PROJEÇÃO ORTOGRÁFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
        // Para simular um "zoom" ortográfico, computamos o valor de "t"
        // utilizando a variável g_CameraDistance.
        float t = 1.5f*g_CameraDistance/2.5f;
        float b = -t;
        float r = t*g_engine_controller.get_screen_ratio();
        float l = -r;
        projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
    }

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // Vamos desenhar 3 instâncias (cópias) do cubo
    for (int i = 1; i <= 3; ++i) {
        // Cada cópia do cubo possui uma matriz de modelagem independente,
        // já que cada cópia estará em uma posição (rotação, escala, ...)
        // diferente em relação ao espaço global (World Coordinates). Veja
        // slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 model;

        if (i == 1) {
            // A primeira cópia do cubo não sofrerá nenhuma transformação
            // de modelagem. Portanto, sua matriz "model" é a identidade, e
            // suas coordenadas no espaço global (World Coordinates) serão
            // *exatamente iguais* a suas coordenadas no espaço do modelo
            // (Model Coordinates).
            model = Matrix_Identity();
        } else if ( i == 2 ) {
            // A segunda cópia do cubo sofrerá um escalamento não-uniforme,
            // seguido de uma rotação no eixo (1,1,1), e uma translação em Z (nessa ordem!).
            model = Matrix_Translate(0.0f, 0.0f, -2.0f) // TERCEIRO translação
                    * Matrix_Rotate(3.141592f / 8.0f, glm::vec4(1.0f,1.0f,1.0f,0.0f)) // SEGUNDO rotação
                    * Matrix_Scale(2.0f, 0.5f, 0.5f); // PRIMEIRO escala
        } else if ( i == 3 ) {
            // A terceira cópia do cubo sofrerá rotações em X,Y e Z (nessa
            // ordem) seguindo o sistema de ângulos de Euler, e após uma
            // translação em X. Veja slides 106-107 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
            model = Matrix_Translate(-2.0f, 0.0f, 0.0f) // QUARTO translação
                    * Matrix_Rotate_Z(g_AngleZ)  // TERCEIRO rotação Z de Euler
                    * Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rotação Y de Euler
                    * Matrix_Rotate_X(g_AngleX); // PRIMEIRO rotação X de Euler

            // Armazenamos as matrizes model, view, e projection do terceiro cubo
            // para mostrar elas na tela através da função TextRendering_ShowModelViewProjection().
            g_the_model = model;
            g_the_projection = projection;
            g_the_view = view;
        }

        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        cube_faces_vao.draw();

        glLineWidth(2.0f);
        cube_axes_vao.draw();
        cube_edges_vao.draw();

        // Desenhamos um ponto de tamanho 15 pixels em cima do terceiro vértice
        // do terceiro cubo. Este vértice tem coordenada de modelo igual à
        // (0.5, 0.5, 0.5, 1.0).
        if ( i == 3 ) {
            glPointSize(15.0f);
            glDrawArrays(GL_POINTS, 3, 1);
        }
    }

    
    glm::mat4 model = Matrix_Identity();
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(10.0f);
    cube_axes_vao.draw();

    glBindVertexArray(0);

    // Pegamos um vértice com coordenadas de modelo (0.5, 0.5, 0.5, 1) e o
    // passamos por todos os sistemas de coordenadas armazenados nas
    // matrizes the_model, the_view, e the_projection; e escrevemos na tela
    // as matrizes e pontos resultantes dessas transformações.
    glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);
}

void update_free_camera_position() {
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.y * g_free_camera_view_unit_vector;
    g_free_camera_position += g_free_camera_speed * g_free_camera_move_vector.x * g_free_camera_right_vector;
}

Vao BuildCubeFaces()
{
    GLfloat face_positions[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
    };

    GLfloat face_colors[] = {
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
        0.0f, 0.5f, 1.0f, 1.0f,
    };

    GLuint face_indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 7, 6,
        6, 5, 4,
        // Top face
        0, 3, 7,
        7, 4, 0,
        // Bottom face
        1, 5, 6,
        6, 2, 1,
        // Left face
        0, 4, 5,
        5, 1, 0,
        // Right face
        3, 2, 6,
        6, 7, 3
    };

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(face_positions), face_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(face_colors), face_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(face_indices), face_indices, GL_STATIC_DRAW)
        .build(GL_TRIANGLES, sizeof(face_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

// Build the edges VAO and register it
Vao BuildCubeEdges()
{
    GLfloat edge_positions[] = {
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
    };

    GLfloat edge_colors[] = {
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
        0.0f,0.0f,0.0f,1.0f,
    };

    GLuint edge_indices[] = {
        0,1, 1,2, 2,3, 3,0, 0,4, 4,7, 7,6, 6,2, 6,5, 5,4, 5,1, 7,3
    };

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(edge_positions), edge_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(edge_colors), edge_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(edge_indices), edge_indices, GL_STATIC_DRAW)
        .build(GL_LINES, sizeof(edge_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

// Build the axes VAO and register it
Vao BuildCubeAxes()
{
    GLfloat axes_positions[] = {
            // X axis
            0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            // Y axis
            0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            // Z axis
            0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
        };

    GLfloat axes_colors[] = {
        1.0f,0.0f,0.0f,1.0f,
        1.0f,0.0f,0.0f,1.0f,
        0.0f,1.0f,0.0f,1.0f,
        0.0f,1.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,
    };

    GLuint axes_indices[] = { 0,1, 2,3, 4,5 };

    return VaoBuilder()
        .add_vbo(0, 4, sizeof(axes_positions), axes_positions, GL_STATIC_DRAW)
        .add_vbo(1, 4, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW)
        .add_ebo(sizeof(axes_indices), axes_indices, GL_STATIC_DRAW)
        .build(GL_LINES, sizeof(axes_indices)/sizeof(GLuint), GL_UNSIGNED_INT);
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
}

void update_free_camera_view_vector() {
    g_free_camera_view_unit_vector = Vec3(
        cosf(g_CameraPhi) * sinf(g_CameraTheta),
        sinf(g_CameraPhi),
        cosf(g_CameraPhi) * cosf(g_CameraTheta)
    );
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (!g_LeftMouseButtonPressed)
        return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = (float)(xpos - g_LastCursorPosX);
    float dy = (float)(ypos - g_LastCursorPosY);

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= 0.01f*dx;

    if (g_camera_is_free) {
        g_CameraPhi   -= 0.01f*dy;
    } else {
        g_CameraPhi   += 0.01f*dy;
    }

    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;

    update_free_camera_view_vector();
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f * (float)yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

void update_free_camera_move_vector() {
    g_free_camera_move_vector = glm::vec2(0.0f, 0.0f);

    if (g_input_move_forward)
        g_free_camera_move_vector.y += 1.0f;
    if (g_input_move_backward)
        g_free_camera_move_vector.y -= 1.0f;
    if (g_input_move_left)
        g_free_camera_move_vector.x -= 1.0f;
    if (g_input_move_right)
        g_free_camera_move_vector.x += 1.0f;

    // Normalize
    if (g_free_camera_move_vector != glm::vec2(0.0f, 0.0f))
        g_free_camera_move_vector = glm::normalize(g_free_camera_move_vector);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod) {
    // =======================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // =======================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    float delta = 3.141592f / 16.0f; // 22.5 graus, em radianos.

    switch (key) {
        case GLFW_KEY_X:
            if (action == GLFW_PRESS) {
                g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_Y:
            if (action == GLFW_PRESS) {
                g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_Z:
            if (action == GLFW_PRESS) {
                g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                g_AngleX = 0.0f;
                g_AngleY = 0.0f;
                g_AngleZ = 0.0f;
            }
            break;
        case GLFW_KEY_P:
            if (action == GLFW_PRESS) {
                g_UsePerspectiveProjection = true;
            }
            break;
        case GLFW_KEY_O:
            if (action == GLFW_PRESS) {
                g_UsePerspectiveProjection = false;
            }
            break;
        case GLFW_KEY_H:
            if (action == GLFW_PRESS) {
                g_ShowInfoText = !g_ShowInfoText;
            }
            break;
        case GLFW_KEY_F:
            if (action == GLFW_PRESS)
                g_camera_is_free = !g_camera_is_free;
            break;
        case GLFW_KEY_W:
            if (action == GLFW_PRESS) {
                g_input_move_forward = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_forward = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS) {
                g_input_move_backward = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_backward = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS) {
                g_input_move_left = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_left = false;
                update_free_camera_move_vector();
            }
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS) {
                g_input_move_right = true;
                update_free_camera_move_vector();
            } else if (action == GLFW_RELEASE) {
                g_input_move_right = false;
                update_free_camera_move_vector();
            }
            break;
    }
}
