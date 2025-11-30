#version 330 core

uniform vec4 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

void main() {
    gl_Position = projection * view * model * position;
    gl_PointSize = point_size;
}
