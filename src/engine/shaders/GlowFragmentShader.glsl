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
    
    // For stellar corona: glow is strongest at center, fades outward
    // Use NdotV directly - when looking at center, NdotV is high (bright)
    // When looking at edges, NdotV is low (dim/transparent)
    float NdotV = max(0.0, dot(n, v));
    
    // Apply falloff to control how quickly the glow fades
    // Higher NdotV (center) = more glow, lower NdotV (edges) = less glow
    float glow = pow(NdotV, glow_falloff);
    
    vec3 glow_final = glow_color * glow * glow_intensity;
    
    color.rgb = glow_final;
    color.a = glow * 0.5;  // Alpha follows the same pattern for smooth blending
}
