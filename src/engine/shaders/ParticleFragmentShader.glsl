#version 330 core

out vec4 FragColor;
uniform vec4 absolute_color;

void main() {
    FragColor = absolute_color;
}
