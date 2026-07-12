#version 460 core

const uint CAMERA_UBO_BINDING_POINT = 0;
const uint TRANSFORM_SSBO_BINDING_POINT = 1;

layout(std140, binding = CAMERA_UBO_BINDING_POINT) uniform Camera
{
    mat4 perspective;
    mat4 view;
    vec3 position;
}camera;

struct ModelMatricesData
{
    mat4 transformMatrix;
    mat4 normalMatrix;
};

layout(std430, binding = TRANSFORM_SSBO_BINDING_POINT) readonly buffer ModelMatrices
{
    ModelMatricesData models[];
};

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_texcoord;
layout(location = 4) in float a_material;

out VERTEX
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    float material;
    mat3 tbn_matrix;
}vertex;

void main()
{
    ModelMatricesData model = models[gl_DrawID];
    vec4 position = model.transformMatrix * vec4(a_position, 1.0);
    gl_Position = camera.perspective * camera.view * position;
    vertex.position = position.xyz;
    vertex.texcoord = a_texcoord;

    // Calculate normal, tangent, bitangent
    vertex.normal = normalize(mat3(model.normalMatrix) * a_normal);
    vec3 tangent = mat3(model.normalMatrix) * a_tangent;
    tangent = normalize(tangent - dot(tangent, vertex.normal) * vertex.normal);
    vec3 bitangent = cross(vertex.normal, tangent);

    vertex.texcoord = a_texcoord;

    vertex.material = a_material;

    vertex.tbn_matrix = mat3(tangent, bitangent, vertex.normal);
}