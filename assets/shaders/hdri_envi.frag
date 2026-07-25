#version 460 core

const vec2 REMAP_COORDS = vec2(0.1591549, 0.3183098);

layout(location = 0) in vec3 v_texcoord; 

layout(binding = 0) uniform sampler2D u_hdri_envi;

layout(location = 0) out vec4 f_color;

vec2 equi_rect_to_texcoord(vec3 value)
{
    value = normalize(value);
    vec2 phy_theta = vec2(atan(value.z, value.x), asin(value.y));
    return fma(phy_theta, REMAP_COORDS, vec2(0.5));
}

void main()
{
    vec2 texcoord = equi_rect_to_texcoord(v_texcoord);
    f_color = texture(u_hdri_envi, texcoord);
}