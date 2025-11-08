#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;
layout (location = 3) in vec4 position;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float alpha;
uniform vec4 absolute_color;

uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float Ns;

out vec4 interpolated_color;
out vec2 texcoords;
out vec3 lambert_diffuse_term;

float max(float a, float b)
{
    return (a > b) ? a : b;
}

void main()
{
    gl_Position = projection * view * model * model_coefficients;

    vec4 normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 p = position;
    vec4 n = normalize(normal); // normal do vertice
    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0)); // vetor da luz para o ponto
    vec4 v = normalize(camera_position - p); // vetor do ponto para a câmera
    vec4 r = -l + 2*n*dot(n,l); // vetor de reflexão especular ideal
    vec4 h = normalize(l + v); // half-way entre l e v
    vec3 I = vec3(1.0,1.0,1.0); // espectro da fonte de luz
    vec3 Ia = vec3(0.2, 0.2, 0.2); // espectro da luz ambiente
    lambert_diffuse_term = I*max(0.0, dot(n,l)); // termo difuso de Lambert
    vec3 ambient_term = Ka*Ia; // termo ambiente

    vec3 blinn_phong_specular_term  = Ks*I*pow(max(0.0, dot(n,h)), Ns);
    interpolated_color.a = 1.0;
    interpolated_color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
    interpolated_color.rgb = pow(interpolated_color.rgb, vec3(1.0,1.0,1.0)/2.2);

    texcoords = texture_coefficients;
}

