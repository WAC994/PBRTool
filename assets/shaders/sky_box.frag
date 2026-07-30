#version 460 core

layout(location = 0) in vec3 v_texcoord; 

layout(binding = 0) uniform samplerCube u_sky_texture;
layout(location = 0) uniform float u_scroll;
layout(location = 1) uniform uint u_pre_filter_levels;

layout(location = 0) out vec4 f_color;

void main()
{
    float lod = u_scroll * (u_pre_filter_levels - 1);
    f_color = texture(u_sky_texture, normalize(v_texcoord), lod);
}