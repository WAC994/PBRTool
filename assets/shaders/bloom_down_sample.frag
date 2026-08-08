#version 460 core

layout(location = 0) in vec2 v_texcoord;

/* UNIFORMS */
layout (binding = 0) uniform sampler2D u_texture;
layout (location = 0) uniform float u_lod; 
layout (location = 1) uniform vec2 u_texel;
layout (location = 2) uniform bool u_karis_avg;

/* OUT */
layout (location = 0) out vec3 f_color;

/* FUNCTIONS */
float luminance(vec3 c)
{
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}

vec3 karis_weight(vec3 c)
{
    // Brian Karis, "Tone Mapping" (SIGGRAPH 2014) — dampens outlier/firefly contribution
    float luma = luminance(c) * 0.25;
    return c / (1.0 + luma);
}

vec3 down_sample(sampler2D tex, vec2 uv, vec2 texel, float lod)
{
    // Kawase filter
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    
    // Outer fetches
    vec3 a = textureLod(tex,  uv + (texel * vec2(-2.0,  2.0)), lod).rgb;
    vec3 b = textureLod(tex,  uv + (texel * vec2( 0.0,  2.0)), lod).rgb;
    vec3 c = textureLod(tex,  uv + (texel * vec2( 2.0,  2.0)), lod).rgb;

    vec3 d = textureLod(tex,  uv + (texel * vec2(-2.0,  0.0)), lod).rgb;
    vec3 e = textureLod(tex,  uv + (texel * vec2( 0.0,  0.0)), lod).rgb;
    vec3 f = textureLod(tex,  uv + (texel * vec2( 2.0,  0.0)), lod).rgb;
    
    vec3 g = textureLod(tex,  uv + (texel * vec2(-2.0, -2.0)), lod).rgb;
    vec3 h = textureLod(tex,  uv + (texel * vec2( 0.0, -2.0)), lod).rgb;
    vec3 i = textureLod(tex,  uv + (texel * vec2( 2.0, -2.0)), lod).rgb;

    // Center fetch
    vec3 j = textureLod(tex,  uv + (texel * vec2(-1.0,  1.0)), lod).rgb;
    vec3 k = textureLod(tex,  uv + (texel * vec2( 1.0,  1.0)), lod).rgb;
    vec3 l = textureLod(tex,  uv + (texel * vec2(-1.0, -1.0)), lod).rgb;
    vec3 m = textureLod(tex,  uv + (texel * vec2( 1.0, -1.0)), lod).rgb;

    // sums of fetches (weighted on influence to final sum = 1.0)
    vec3 down_sample = vec3(0.0);
    if (u_karis_avg)
    {
        vec3 top_left, top_right, center, bottom_left, bottom_right;
        top_left     = (a + b + d + e) * (0.125 / 4.0);
        top_right    = (b + c + e + f) * (0.125 / 4.0);
        center       = (j + k + l + m) * (0.5 / 4.0);
        bottom_left  = (d + e + g + h) * (0.125 / 4.0);
        bottom_right = (e + f + h + i) * (0.125 / 4.0);
        down_sample = karis_weight(top_left) + karis_weight(top_right) + karis_weight(center) + karis_weight(bottom_left) + karis_weight(bottom_right);
    }
    else
    {
        down_sample = e * 0.125;
        down_sample += (a + c + g + i) * 0.03125;
        down_sample += (b + d + f + h) * 0.0625;
        down_sample += (j + k + l + m) * 0.125;
    }
    
    return max(down_sample, 1e-5);
}

void main()
{
    f_color = down_sample(u_texture, v_texcoord, u_texel, u_lod);
}