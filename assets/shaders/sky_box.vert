#version 460 core

const uint CAMERA_UBO_BINDING_POINT = 0;
const uint TRANSFORM_UBO_BINDING_POINT = 3;

layout(std140, binding = CAMERA_UBO_BINDING_POINT) uniform CameraData
{
    mat4 perspective;
    mat4 view;
    vec3 position;
}camera;

layout(location = 0) in vec3 a_position;

layout(location = 0) out vec3 v_texcoord; 

void main()
{
    mat4 view = mat4(mat3(camera.view)); 
    vec4 position = camera.perspective * view * vec4(a_position, 1.0);
    gl_Position = position.xyww;
    v_texcoord = a_position;
}