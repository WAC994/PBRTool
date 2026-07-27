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

vec3 importance_sampling_trowbridge_reitz(vec2 distribution, vec3 normal, float alpha2)
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

float trowbridge_reitz_ggx(float ndoth, float alpha2)
{
    float denom = (ndoth * alpha2 - ndoth) * ndoth + 1;
    return alpha2 / (PI * denom * denom);
}


layout(location = 0) in vec3 v_texcoord; 

layout(binding = 0) uniform samplerCube u_envi_cube_map;
layout(location = 0) uniform float u_alpha2;
layout(location = 1) uniform float u_resolution;

layout(location = 0) out vec3 f_color;

void main()
{
    vec3 normal = normalize(v_texcoord);
    vec3 reflect_dir = normal;
    vec3 view_dir = reflect_dir;

    float total_weight = 0.0;
    vec3 pre_filter = vec3(0.0);
    for (uint i = 0; i < MAX_SAMPLES; i++)
    {
        vec2 distribution = hammersley(i, MAX_SAMPLES);
        vec3 halfway_dir = importance_sampling_trowbridge_reitz(distribution, normal, u_alpha2);

        float vdoth = dot(view_dir, halfway_dir);
        vec3 light_dir = normalize(2.0 * vdoth * halfway_dir - view_dir);
        float ndotl = max(dot(normal, light_dir), 0.0);

        if (ndotl > 0.0)
        {
            float ndoth = max(dot(normal, halfway_dir), 0.0);
            float normal_distribution = trowbridge_reitz_ggx(ndoth, u_alpha2);
            float probability_distribution_function = (normal_distribution * ndoth / (4.0 * vdoth)) + 1e-4;
            
            float mip_texel = 4.0 * PI / (6.0 * pow(u_resolution, 2.0));
            float mip_sample = 1.0 / (float(MAX_SAMPLES) * probability_distribution_function + 1e-5);

            float mipLevel = u_alpha2 == 0.0 ? 0.0 : 0.5 * log2(mip_sample / mip_texel); 

            pre_filter += texture(u_envi_cube_map, light_dir, mipLevel).rgb * ndotl;
            total_weight += ndotl;
        }
    }
    
    f_color = pre_filter / total_weight;
}