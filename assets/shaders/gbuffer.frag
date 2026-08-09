#version 460 core
#extension GL_ARB_bindless_texture : require

const uint NONE_FLAG  = 0x0;
const uint COLOR_FLAG = 0x1;
const uint ORMS_FLAG  = 0x2;
const uint NORM_FLAG  = 0x4;
const uint EMIT_FLAG  = 0x8;

const uint MATERIAL_SSBO_BINDING_POINT = 2;

struct MaterialData
{
    layout(bindless_sampler) sampler2D color_tex;
    layout(bindless_sampler) sampler2D orms_tex;
    layout(bindless_sampler) sampler2D normal_tex;
    layout(bindless_sampler) sampler2D emission_tex;
    vec4 color;
    vec4 emission;
    float occlusion;
    float roughness;
    float metallic;
    float specular;
    uint tex_flags;
};

layout(std430, binding = MATERIAL_SSBO_BINDING_POINT) readonly buffer Materials
{
    MaterialData materials[];
};

layout(location = 0) in VERTEX
{
    vec3 position;
    vec3 normal;
    vec3 view_dir;
    vec2 texcoord;
    flat float material;
    mat3 tbn_matrix;
}vertex;

float useMaterial(uint textureflag, uint textureflags)
{
    return clamp(float(textureflag & textureflags), 0.0, 1.0);
}

vec4 linear_to_sRGB(vec4 color)
{
    vec3 lowValue = color.rgb * 12.92;
    precise vec3 highValue = fma(vec3(1.055), pow(color.rgb, vec3(0.41666)), vec3(-0.055));
    color.rgb = mix(lowValue, highValue, greaterThan(color.rgb, vec3(0.0031308)));
    return color;
}

layout(location = 0) out vec4 g_color; // GL_SRGB8_ALPHA8
layout(location = 1) out vec4 g_orms; // GL_RGBA16F
layout(location = 2) out vec3 g_emission; // GL_R11F_G11F_B10F
layout(location = 3) out vec3 g_position; // GL_RGB16F
layout(location = 4) out vec3 g_normal; // GL_RGB16F
layout(location = 5) out vec3 g_view_dir; // GL_RGB16F

void main()
{
    MaterialData mat = materials[uint(vertex.material)];

    g_color = mix(linear_to_sRGB(mat.color),
        texture(mat.color_tex, vertex.texcoord),
        useMaterial(COLOR_FLAG, mat.tex_flags));

    g_emission = mix(linear_to_sRGB(mat.emission).rgb * mat.emission.a,
        texture(mat.emission_tex, vertex.texcoord).rgb * mat.emission.a,
        useMaterial(EMIT_FLAG, mat.tex_flags));

    // If reflectance in alpha channel of the ORMR texture is 1.0 (White) use the refectance float
    vec4 orms_tex = texture(mat.orms_tex, vertex.texcoord);
    orms_tex = vec4(orms_tex.rgb, orms_tex.a == 1.0 ? mat.specular : orms_tex.a);
    g_orms = mix(vec4(mat.occlusion, mat.roughness, mat.metallic, mat.specular),
        orms_tex,
        useMaterial(ORMS_FLAG, mat.tex_flags));
    
    g_position = vertex.position;

    g_normal = mix(normalize(vertex.normal),
        normalize(vertex.tbn_matrix * (texture(mat.normal_tex, vertex.texcoord).xyz * 2.0 - 1.0)),
        useMaterial(NORM_FLAG, mat.tex_flags));

    g_view_dir = vertex.view_dir;
}