#version 460 core

const uint CAMERA_UBO_BINDING_POINT = 0;

layout(std140, binding = CAMERA_UBO_BINDING_POINT) uniform CameraData
{
    mat4 perspective;
    mat4 view;
    vec3 position;
}camera;

layout(location = 0) in vec3 a_position;


out VERTEX
{
    vec3 position;
}vertex;

void main()
{
    gl_Position = camera.perspective * camera.view * vec4(a_position, 1.0);
}