#version 330 core


in vec4 interpolated_color;
uniform sampler2D TextureImage;
in vec2 texcoords;
in vec3 lambert_diffuse_term;
in vec3 ambient_term;

out vec4 color;

void main()
{
    vec3 Kd = texture(TextureImage, vec2(texcoords.x, texcoords.y)).rgb;
    color = vec4(Kd * lambert_diffuse_term + ambient_term, 1.0);
}

