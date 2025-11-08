#version 330 core


in vec4 interpolated_color;
uniform sampler2D TextureImage;
in vec2 texcoords;

out vec4 color;

void main()
{
    vec3 Kd = texture(TextureImage, vec2(texcoords.x, texcoords.y)).rgb;
    color = vec4(Kd, 1.0) * interpolated_color;
}

