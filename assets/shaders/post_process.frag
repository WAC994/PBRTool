#version 460 core

const float BLOOM_FACTOR = 0.04;

layout(location = 0) in vec2 v_texCoord;

layout(location = 0) out vec4 f_color;

layout(binding = 0) uniform sampler2D u_hdr_texture;
layout(binding = 1) uniform sampler2D u_bloom_texture;

// Uchimura 2017 "HDR Theory and Practice" (GT tonemap)
// P: max display brightness, a: contrast, m: linear section start,
// l: linear section length, c: black tightness, b: pedestal/black offset
vec3 uchimura(vec3 x, float P, float a, float m, float l, float c, float b) {
    float l0 = ((P - m) * l) / a;
    float L0 = m - m / a;
    float L1 = m + (1.0 - m) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = -C2 / P;

    vec3 w0 = vec3(1.0 - smoothstep(0.0, m, x));
    vec3 w2 = vec3(step(m + l0, x));
    vec3 w1 = vec3(1.0 - w0 - w2);

    vec3 T = vec3(m * pow(x / m, vec3(c)) + b);   // toe
    vec3 S = vec3(P - (P - S1) * exp(CP * (x - S0))); // shoulder
    vec3 L = vec3(m + a * (x - m));                // linear mid

    return T * w0 + L * w1 + S * w2;
}

vec3 uchimura_tone_mapper(vec3 x) {
    const float P = 1.0;   // max brightness
    const float a = 1.0;   // contrast
    const float m = 0.22;  // linear section start
    const float l = 0.4;   // linear section length
    const float c = 1.33;  // black tightness
    const float b = 0.0;   // pedestal
    return uchimura(x, P, a, m, l, c, b);
}

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
    color = uchimura_tone_mapper(color);
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