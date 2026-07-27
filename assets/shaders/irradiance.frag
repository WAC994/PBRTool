#version 460 core

const float PI = 3.1415926;
const float ANGLE_360 = 2.0 * PI;
const float ANGLE_90 = 0.5 * PI;

layout(location = 0) in vec3 v_texcoord; 

layout(binding = 0) uniform samplerCube u_envi_cube_map;

layout(location = 0) out vec3 f_color;

void main()
{
    vec3 surface_normal = normalize(v_texcoord);

    // Create world space transform
    vec3 world_up_dir = vec3(0.0, 1.0, 0.0);
    vec3 right_dir = normalize(cross(world_up_dir, surface_normal));
    vec3 up_dir = normalize(cross(surface_normal, right_dir));
    mat3 world_space = mat3(right_dir, up_dir, surface_normal);

    const float angle_delta = 0.0125;
    uint sample_count;

    vec3 irradiance = vec3(0.0);
    for (float phi = 0.0; phi < ANGLE_360; phi += angle_delta)
    {
        for(float theta = 0.0; theta < ANGLE_90; theta += angle_delta)
        {
            // Convert the angles(spherical coordinates) to cartasian coordinates
            vec3 texcoord = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            // Transform the tangent-space coordinates to world-space
            texcoord = world_space * texcoord;

            // Use the world-space coordinate to sample texels from the environment cube map
            // and weigh sign theta 
            irradiance += texture(u_envi_cube_map, texcoord).rgb * cos(theta) * sin(theta);

            sample_count++;
        }
    }
    f_color = PI * irradiance * (1.0 / float(sample_count));
}