#version 460 core

layout(location = 0) uniform bool u_switch;
layout(location = 1) uniform uint u_pre_filter_levels;

const float PI = 3.1415926;
const float INVPI = 0.3183098;
const float INV_ENERGY_FACTOR = 0.6622516;
const float AVERAGE_REFLECTANCE = 0.04;

const uint LIGHTS_SSBO_BINDING_POINT = 4;

struct LightData
{
    float r, g, b;
    float x, y, z;
};

layout(std430, binding = LIGHTS_SSBO_BINDING_POINT) readonly buffer Lights
{
    uint light_count;
    LightData lights[];
};

layout(location = 0) in vec2 v_texcoord;

layout(binding = 0) uniform sampler2D g_color;
layout(binding = 1) uniform sampler2D g_orms;
layout(binding = 2) uniform sampler2D g_emission;
layout(binding = 3) uniform sampler2D g_position;
layout(binding = 4) uniform sampler2D g_normal;
layout(binding = 5) uniform sampler2D g_view_dir;

layout(binding = 6) uniform samplerCube u_irradiance_cube_map;
layout(binding = 7) uniform samplerCube u_pre_filter_cube_map;
layout(binding = 8) uniform sampler2D u_BRDF_LUT;

struct Surface
{
    vec3 base_color;
    vec3 emission;
    vec3 position;
    vec3 normal;
    vec3 view_dir;
    vec3 f0;
    float occlusion, spec_occlusion;
    float roughness, alpha, alpha2;
    float metallic;
    float ndotv;
};

vec3 fresnel_schlick(vec3 f0, vec3 f90, float costheta)
{
    return f0 + (f90 - f0) * exp2((-5.55473 * costheta - 6.98316) * costheta); // using approximation for pow(costheta, 5.0)
}

float trowbridge_reitz_ggx(float ndoth, float alpha2)
{
    float denom = (ndoth * alpha2 - ndoth) * ndoth + 1;
    return alpha2 / (PI * denom * denom);
}

float smith_ggx_fast(float ndotv, float ndotl, float alpha)
{
    float ggxv_lambda = ndotl * (ndotv * (1.0 - alpha) + alpha);
    float ggxl_lambda = ndotv * (ndotl * (1.0 - alpha) + alpha);
    return 0.5 / ( ggxv_lambda + ggxl_lambda );
}

float DICE_Disney_diffuse(float ndotv, float ndotl, float ldoth, float roughness)
{
    float energy_bias = mix(0.0, 0.5, roughness);
    float energy_factor = mix(1.0, INV_ENERGY_FACTOR, roughness);
    float f90 = energy_bias * 2.0 * ldoth * ldoth * roughness;
    float light_scatter = fresnel_schlick(vec3(1.0), vec3(f90), ndotl).r; 
    float view_scatter = fresnel_schlick(vec3(1.0), vec3(f90), ndotv).r;
    return light_scatter * view_scatter * energy_factor;
}

vec3 point_lighting(LightData light, Surface surface)
{
    vec3 light_col = vec3(light.r, light.g, light.b);
    vec3 light_pos = vec3(light.x, light.y, light.z);
    vec3 light_vec = light_pos - surface.position;
    vec3 light_dir = normalize(light_vec);
    vec3 halfway_dir = normalize(light_dir + surface.view_dir);
    float light_dis = length(light_vec);
    float attenuation = 1.0 / pow(light_dis, 2.0);
    vec3 radiance = light_col * attenuation;

    float ndotl = max(dot(surface.normal, light_dir), 0.0);
    float ldoth = max(dot(light_dir, halfway_dir), 0.0);
    float ndoth = max(dot(surface.normal, halfway_dir), 0.0);

    vec3 diffuse = surface.base_color * DICE_Disney_diffuse(surface.ndotv, ndotl, ldoth, surface.roughness) * surface.occlusion * INVPI;

    vec3 fresnel = fresnel_schlick(surface.f0, vec3(1.0), ldoth);

    float normal_distribution = trowbridge_reitz_ggx(ndoth, surface.alpha2);

    float geometery = smith_ggx_fast(surface.ndotv, ndotl, surface.alpha);

    vec3 diffuse_constant = vec3(1.0) - fresnel;
    diffuse_constant *= 1.0 - surface.metallic;

    vec3 specular = fresnel * normal_distribution * geometery * surface.spec_occlusion;

    return (diffuse_constant * diffuse + specular) * radiance * ndotl;
}

float specular_occlusion(float ndotv, float occlusion, float roughness)
{
    return max(pow(ndotv + occlusion, exp2(-16.0 * roughness - 1.0)) - 1.0 + occlusion, 0.0);    
}

layout(location = 0) out vec3 f_color;

void main()
{
    Surface surface;
    surface.base_color = texture(g_color, v_texcoord).rgb;
    surface.emission = texture(g_emission, v_texcoord).rgb;
    surface.position = texture(g_position, v_texcoord).xyz;
    surface.normal = texture(g_normal, v_texcoord).xyz;
    surface.view_dir = texture(g_view_dir, v_texcoord).xyz;
    vec4 orms = texture(g_orms, v_texcoord);
    surface.occlusion = orms.r;
    surface.roughness = orms.g;
    surface.alpha = pow(surface.roughness, 2.0);
    surface.alpha2 = pow(surface.alpha, 2.0);
    surface.metallic = orms.b;
    float spec_reflectance_scale = orms.a * orms.a;
    float f0 = (0.16 * spec_reflectance_scale); 
    surface.f0 = mix(vec3(f0), surface.base_color, surface.metallic);
    surface.ndotv = max(dot(surface.normal, surface.view_dir), 1e-4);
    surface.spec_occlusion = specular_occlusion(surface.ndotv, surface.occlusion, surface.alpha);

    // Discrete lighting
    vec3 output_radiance = vec3(0.0);
    for (uint i = 0; i < light_count; i++)
    {
        output_radiance += point_lighting(lights[i], surface);
    }
    output_radiance += surface.emission;

    // Image based lighting
    // Specular IBL
    vec3 roughness_factor = max(vec3(1.0 - surface.roughness), surface.f0);
    vec3 specular_constant = fresnel_schlick(surface.f0, roughness_factor, surface.ndotv);
    vec3 reflect_dir = reflect(-surface.view_dir, surface.normal);

    float pre_filter_lod = float(u_pre_filter_levels - 1);
    vec3 prefilter = textureLod(u_pre_filter_cube_map, reflect_dir, surface.roughness * pre_filter_lod).rgb;
    vec2 envi_bdrf = texture(u_BRDF_LUT, vec2(surface.ndotv, surface.roughness)).rg;
    vec3 specular = prefilter * (specular_constant * envi_bdrf.x + envi_bdrf.y) * surface.spec_occlusion;

    // Diffuse IBL
    vec3 irradiance = texture(u_irradiance_cube_map, surface.normal).rgb * surface.occlusion;
    vec3 diffuse_constant = (1.0 - specular_constant) * (1 - surface.metallic);
    vec3 diffuse = surface.base_color * irradiance;

    vec3 ibl_radiance = diffuse_constant * diffuse + specular;

    f_color = ibl_radiance + output_radiance;
}