#version 330 core

layout(location = 0) in vec4 model_coefficients;
layout(location = 1) in float point_size;      
layout(location = 2) in float point_alpha;      

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 position_world;
out float v_alpha;

void main() {
    position_world = model * model_coefficients;
    gl_Position = projection * view * position_world;
    gl_PointSize = point_size;
    v_alpha = point_alpha;
}
