#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 norm;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 light_space_matrix;

out vec2 uv_frag;
out vec3 normal_interp;
out vec3 frag_pos;
out vec3 frag_unprojected_pos;
out vec4 frag_pos_in_light_space;
out float height_map_scale_factor;

void main()
{
    uv_frag = uv;
    normal_interp = normalize(norm * vec4(normal, 0.0f)).rgb;
    frag_unprojected_pos = (world * vec4(position, 1.0f)).rgb;
    frag_pos_in_light_space = light_space_matrix * vec4(frag_unprojected_pos.rgb, 1.0f);
    gl_Position = proj * view * vec4(frag_unprojected_pos, 1.0f);
    height_map_scale_factor = position.y;
    frag_pos = gl_Position.rgb;

}
