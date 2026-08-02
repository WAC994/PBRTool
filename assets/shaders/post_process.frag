#version 460 core

const float BLOOM_FACTOR = 0.04;

layout(location = 0) in vec2 v_texCoord;

layout(location = 0) out vec4 f_color;

layout(binding = 0) uniform sampler2D u_hdr_texture;
layout(binding = 1) uniform sampler2D u_bloom_texture;


vec3 ACES_tone_mapper(vec3 color) 
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
}

float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 reinhard_luminance(vec3 color) {
    float luminance_balance = luminance(color);
    float output_color = luminance_balance / (1.0 + luminance_balance);
    return color * (output_color / max(luminance_balance, 1e-5));
}

vec4 color_correction(vec3 color)
{
    color = ACES_tone_mapper(color);
    // color = reinhard_luminance(color);
    vec3 lowValue = color * 12.92;
    precise vec3 highValue = fma(vec3(1.055), pow(color, vec3(0.41666)), vec3(-0.055));
    color = mix(lowValue, highValue, greaterThan(color, vec3(0.0031308)));
    return vec4(color, 1.0);
}

void main()
{
    vec3 color = texture(u_hdr_texture, v_texCoord).rgb;
    vec3 bloom = texture(u_bloom_texture, v_texCoord).rgb;

    f_color = color_correction(mix(color, bloom, BLOOM_FACTOR));
}