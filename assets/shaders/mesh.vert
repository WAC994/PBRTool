#version 460 core

const uint CAMERA_UBO_BINDING_POINT = 0;

layout(std140, binding = CAMERA_UBO_BINDING_POINT) uniform CameraData
{
    mat4 perspective;
    mat4 view;
    vec3 position;
}camera;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_texcoord;
layout(location = 4) in float a_material;

layout(location = 0) out vec2 v_texcoord;

void main()
{
    gl_Position = camera.perspective * camera.view * vec4(a_position, 1.0);
    v_texcoord = a_texcoord;
}