#version 460 core
#extension GL_ARB_bindless_texture : require

const uint MATERIAL_SSBO_BINDING_POINT = 2;

struct MaterialData
{
    layout(bindless_sampler) sampler2D color_tex;
    layout(bindless_sampler) sampler2D orms_tex;
    layout(bindless_sampler) sampler2D normal_tex;
    layout(bindless_sampler) sampler2D emission_tex;
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

layout(location = 0) out vec4 f_color;

void main()
{
    MaterialData material = materials[uint(vertex.material)];
    f_color = texture(material.orms_tex, vertex.texcoord);
}