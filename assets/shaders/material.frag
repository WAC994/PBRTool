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

layout(bindless_sampler) uniform sampler2D  u_texture;

void main()
{
    uint material_id = uint(vertex.material);
    f_color = texture(materials[material_id].color_tex, vertex.texcoord);
    // f_color = vec4(1.0, 0.5 ,0.25, 1.0);
}