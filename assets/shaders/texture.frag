#version 460 core

layout(location = 0) in vec2 v_texcoord;

layout(binding = 0) uniform sampler2D u_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    f_color = textureLod(u_texture, v_texcoord, 0);
    // f_color = vec4(v_texcoord, 1.0, 1.0);
    // f_color = vec4(0.0117, 0.1921, 0.3215, 1.0);
}