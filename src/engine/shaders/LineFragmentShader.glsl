#version 330 core

uniform vec4 absolute_color;
out vec4 color;

void main() {
    color = absolute_color;
}
