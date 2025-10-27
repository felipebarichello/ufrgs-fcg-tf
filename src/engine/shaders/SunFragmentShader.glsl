#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
in vec4 position_world;
in vec4 normal;

// Cor base do sol
uniform vec3 sun_color;
// Intensidade do brilho base
uniform float base_intensity;
// Intensidade do brilho Fresnel (rim lighting)
uniform float fresnel_intensity;
// Expoente de Fresnel (controla a largura da borda brilhante)
uniform float fresnel_power;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 view;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que pertence à superfície do sol
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador
    vec4 n = normalize(normal);

    // Vetor que define o sentido da câmera em relação ao ponto atual
    vec4 v = normalize(camera_position - p);

    // Cálculo do efeito Fresnel
    // O efeito Fresnel faz com que a borda do sol brilhe mais intensamente
    // quando vista de um ângulo rasante (grazing angle)
    float fresnel = pow(1.0 - max(0.0, dot(n, v)), fresnel_power);
    
    // Cor final do sol combinando:
    // 1. Brilho base (emissivo) - o sol sempre brilha
    // 2. Brilho Fresnel - intensifica nas bordas para criar o efeito de halo/glow
    vec3 emissive = sun_color * base_intensity;
    vec3 fresnel_glow = sun_color * fresnel * fresnel_intensity;
    
    color.rgb = emissive + fresnel_glow;
    color.a = 1.0;

    // Cor final com correção gamma, considerando monitor sRGB
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}
