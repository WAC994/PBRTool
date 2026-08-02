#version 460 core
const float BLUR_BIAS = 0.0005;

layout (location = 0) in vec2 v_texcoord;

layout (binding = 0) uniform sampler2D u_blur_texture;
layout (binding = 1) uniform sampler2D u_add_texture;

layout (location = 0) uniform float u_blur_lod;
layout (location = 1) uniform float u_add_lod;
layout (location = 2) uniform float u_block_add;

layout (location = 0) out vec3 f_Color;

vec3 tent_filter_blur(sampler2D tex, vec2 uv, float bias, float lod)
{
    // tent filter
    // 1 - 2 - 1 
    // |   |   |
    // 2 - 4 - 2
    // |   |   |
    // 1 - 2 - 1
    
    vec3 tent_filter_blur = vec3(0.0);
    tent_filter_blur += 1.0 * textureLod(tex,  uv + (bias * vec2(-1.0,  1.0)), lod).rgb;
    tent_filter_blur += 2.0 * textureLod(tex,  uv + (bias * vec2( 0.0,  1.0)), lod).rgb;
    tent_filter_blur += 1.0 * textureLod(tex,  uv + (bias * vec2( 1.0,  1.0)), lod).rgb;

    tent_filter_blur += 2.0 * textureLod(tex,  uv + (bias * vec2(-1.0,  0.0)), lod).rgb;
    tent_filter_blur += 4.0 * textureLod(tex,  uv + (bias * vec2( 0.0,  0.0)), lod).rgb;
    tent_filter_blur += 2.0 * textureLod(tex,  uv + (bias * vec2( 1.0,  0.0)), lod).rgb;
    
    tent_filter_blur += 1.0 * textureLod(tex,  uv + (bias * vec2(-1.0, -1.0)), lod).rgb;
    tent_filter_blur += 2.0 * textureLod(tex,  uv + (bias * vec2( 0.0, -1.0)), lod).rgb;
    tent_filter_blur += 1.0 * textureLod(tex,  uv + (bias * vec2( 1.0, -1.0)), lod).rgb;
    return tent_filter_blur / 16.0;
}

void main()
{
    vec3 blurred_color = tent_filter_blur(u_blur_texture, v_texcoord, BLUR_BIAS, u_blur_lod);
    vec3 added_color = textureLod(u_add_texture, v_texcoord, u_add_lod).rgb * u_block_add;
    f_Color = blurred_color + added_color;
}