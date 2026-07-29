#version 460 core

const uint MAX_SAMPLES = 1024;
const float PI = 3.1415926;
const float ANGLE_360 = 2.0 * PI;
const float ANGLE_90 = 0.5 * PI;

float radical_inverse_Van_Der_Corput(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint n)
{
    return vec2(float(i) / float (n), radical_inverse_Van_Der_Corput(i));
}

vec3 importance_sample_trowbridge_reitz(vec2 distribution, vec3 normal, float alpha2)
{
    // Generate halfway direction spherical cordinates from the Monte-Carlo Distribution and specular lobe
    float phi = ANGLE_360 * distribution.x;
    float cos_theta = sqrt((1.0 - distribution.y) / (1.0 + (alpha2 - 1.0) * distribution.y));
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    // Convert halfway direction into cartasian coordinates
    vec3 halfway_dir = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);

    // Generate world space transform
    vec3 up        = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    return normalize(mat3(tangent, bitangent, normal) * halfway_dir);
}

float geometry_schlick_GGX(float ndotv, float k)
{
    return ndotv / (ndotv * (1.0 - k) + k);
}

float geometry_smith(float ndotv, float ndotl, float k)
{
    float ggx2 = geometry_schlick_GGX(ndotv, k);
    float ggx1 = geometry_schlick_GGX(ndotl, k);
    return ggx1 * ggx2;
}

vec2 brdf_integration(float ndotv, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float k = alpha / 2.0;

    vec3 view_dir;
    view_dir.x = sqrt(1.0 - pow(ndotv, 2.0));
    view_dir.y = 0.0;
    view_dir.z = ndotv;

    float a = 0.0;
    float b = 0.0;

    vec3 normal = vec3(0.0, 0.0, 1.0);

    for (uint i = 0; i < MAX_SAMPLES; i++)
    {
        vec2 dis = hammersley(i, MAX_SAMPLES);
        vec3 half_dir = importance_sample_trowbridge_reitz(dis, normal, alpha2);
        vec3 light_dir = normalize(2.0 * dot(view_dir, half_dir) * half_dir - view_dir);

        float ndotl = max(light_dir.z, 0.0);
        float ndoth = max(half_dir.z, 0.0);
        float vdoth = max(dot(view_dir, half_dir), 0.0);

        if (ndotl > 0.0)
        {
            float ndotv2 = max(dot(normal, view_dir), 0.0);
            float ndotl2 = max(dot(normal, light_dir), 0.0);
            float geom = geometry_smith(ndotv2, ndotl2, k);
            float geom_vis = (geom * vdoth) / (ndoth * ndotv);
            float fc = pow(1 - vdoth, 5.0);

            a += (1.0 - fc) * geom_vis;
            b += fc * geom_vis;
        }    
    }
    return vec2(a, b) / float(MAX_SAMPLES);
}

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec2 f_color;

void main()
{
    f_color = brdf_integration(max(v_texcoord.x, 1e-4), v_texcoord.y);
}