#version 460 core

in VERTEX
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    float material;
    mat3 tbn_matrix;
}vertex;

layout(binding = 0) uniform sampler2D u_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    f_color = texture(u_texture, vertex.texcoord);
}