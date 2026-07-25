#version 460 core

const uint CUBE_SIDES_UBO_BINDING_POINT = 5;

layout(std140, binding = CUBE_SIDES_UBO_BINDING_POINT) uniform CaptureData
{
    mat4 perspective;
    mat4 view;
}capture;

layout(location = 0) in vec3 a_position;

layout(location = 0) out vec3 v_texcoord; 

void main()
{
    gl_Position = capture.perspective * capture.view * vec4(a_position, 1.0);
    v_texcoord = a_position;
}