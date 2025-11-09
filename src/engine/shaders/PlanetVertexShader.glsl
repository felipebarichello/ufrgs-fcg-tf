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

uniform vec3 Ks;
uniform float Ns;

out vec4 interpolated_color;
out vec2 texcoords;
out vec3 lambert_diffuse_term;
out vec3 Ia;
out vec3 position_model;

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
    vec3 I = vec3(1.0,1.0,1.0); // espectro da fonte de luz
    Ia = vec3(0.1, 0.1, 0.1); // espectro da luz ambiente
    lambert_diffuse_term = I*max(0.0, dot(n,l)); // termo difuso de Lambert
    texcoords = texture_coefficients;
    position_model = model_coefficients.xyz;
}

