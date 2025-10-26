#version 330 core

in float v_alpha;
uniform vec4 absolute_color;

out vec4 color;

void main() {
    // circular point sprite mask
    vec2 coord = gl_PointCoord * 2.0 - vec2(1.0);
    if (dot(coord, coord) > 1.0) discard;
    float alpha = absolute_color.a * v_alpha;
    color = vec4(absolute_color.rgb, alpha);
}
