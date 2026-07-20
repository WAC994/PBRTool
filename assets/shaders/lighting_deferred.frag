#version 460 core

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

struct Surface
{
    vec3 albedo;
    vec3 position;
    vec3 normal;
    vec3 view_dir;
    float occlusion;
    float roughness, alpha, alpha2;
    float metallic;
    float f0;
    float vdotn;
};

vec3 fresnelSchlick(vec3 f0, vec3 f90, float costheta)
{
    return f0 + (f90 - f0) * exp2((-5.55473 * costheta - 6.98316) * costheta);
}

vec3 point_lighting(LightData light, Surface surface)
{
    vec3 light_color = vec3(light.r, light.g, light.b);
    vec3 light_postition = vec3(light.x, light.y, light.z);
    return light_color;
}

layout(location = 0) out vec4 f_color;

void main()
{
    Surface surface;
    surface.albedo = texture(g_color, v_texcoord).rgb;
    surface.position = texture(g_position, v_texcoord).xyz;
    surface.normal = texture(g_normal, v_texcoord).xyz;
    surface.view_dir = texture(g_view_dir, v_texcoord).xyz;
    vec4 orms = texture(g_orms, v_texcoord);
    surface.occlusion = orms.r;
    surface.roughness = orms.g;
    surface.alpha = pow(surface.roughness, 2.0);
    surface.alpha2 = pow(surface.alpha, 2.0);
    surface.metallic = orms.b;
    surface.f0 = 0.04f;
    surface.vdotn = max(dot(surface.view_dir, surface.normal), 1e-4);

    vec3 output_radiance = vec3(0.0);
    for (uint i = 0; i < light_count; i++)
    {
        output_radiance += point_lighting(lights[i], surface);
    }
    f_color = vec4(output_radiance, 1.0);
}