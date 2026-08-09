#version 460 core

layout(location = 0) in VERTEX
{
    vec3 position;
    vec3 normal;
    vec3 view_dir;
    vec2 texcoord;
    flat float material;
    mat3 tbn_matrix;
}vertex;

layout(binding = 0) uniform sampler2D u_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    f_color = texture(u_texture, vertex.texcoord);
}