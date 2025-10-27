#version 330 core

in vec4 position_world;
in vec4 normal;
in vec3 view_direction;

uniform vec3 glow_color;
uniform float glow_intensity;
uniform float glow_falloff;

out vec4 color;

void main()
{
    vec3 n = normalize(normal.xyz);
    vec3 v = normalize(view_direction);
    
    float NdotV = max(0.0, dot(n, v));
    float glow = pow(1.0 - NdotV, glow_falloff);
    
    vec3 glow_final = glow_color * glow * glow_intensity;
    
    color.rgb = glow_final;
    color.a = glow * 0.6;
}
