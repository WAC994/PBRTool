#version 460 core

layout(location = 0) uniform bool u_switch;

const uint LIGHTS_SSBO_BINDING_POINT = 4;

struct LightData
{
    float r, g, b;
    float x, y, z;
};

layout(std430, binding = LIGHTS_SSBO_BINDING_POINT) readonly buffer Lights
{
    uint light_count;
    LightData lights[];
};

in VERTEX
{
    vec3 position;
    vec3 normal;
    vec3 view_dir;
    vec2 texcoord;
    float material;
    mat3 tbn_matrix;
}vertex;

layout(binding = 0) uniform sampler2D u_texture;

layout(location = 0) out vec4 f_color;

void main()
{
    LightData light = lights[0];
    vec3 light_pos = vec3(light.x, light.y, light.z);
    
    vec3 color = texture(u_texture, vertex.texcoord * 2.0).rgb;
    
    vec3 normal = normalize(vertex.normal);
    vec3 view_dir = normalize(vertex.view_dir);
    vec3 light_dir = normalize(light_pos - vertex.position);
    vec3 reflect_dir = reflect(-light_dir, normal); 
    vec3 halfway_dir = normalize(view_dir + light_dir);

    float vdotr = max(dot(view_dir, reflect_dir), 0.0);
    float ndoth = max(dot(normal, halfway_dir), 0.0);
    vec3 specular = vec3(0.5) * pow(u_switch ? vdotr : ndoth, 64.0);

    float ndotv = max(dot(normal, view_dir), 0.0);
    float ndotl = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = color * ndotl;

    f_color = vec4(diffuse + specular, 1.0f);
}