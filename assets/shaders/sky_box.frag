#version 460 core

layout(location = 0) in vec3 v_texcoord; 

layout(binding = 0) uniform samplerCube u_sky_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    vec3 height = normalize(v_texcoord);
    float gradient = (height.y + 1.0) / 2.0;
    f_color = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.000911, 0.030713, 0.084376, 1.0), pow(gradient, 1.0 / 1.1));
    // f_color = texture(u_sky_texture, normalize(v_texcoord));
}