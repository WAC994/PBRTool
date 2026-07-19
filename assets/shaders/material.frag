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

in VERTEX
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    flat float material;
    mat3 tbn_matrix;
}vertex;

float useMaterial(uint textureflag, uint textureflags)
{
    return clamp(float(textureflag & textureflags), 0.0, 1.0);
}

layout(location = 0) out vec4 f_color;

void main()
{
    MaterialData material = materials[uint(vertex.material)];
    f_color = mix(material.color, texture(material.color_tex, vertex.texcoord), useMaterial(COLOR_FLAG, material.tex_flags));
}