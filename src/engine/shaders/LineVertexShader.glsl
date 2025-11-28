#version 330 core

// Simple line vertex shader: only position attribute is required.
layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * position;
}
