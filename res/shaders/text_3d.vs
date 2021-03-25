#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

out vec2 uv_frag;

void main()
{
    uv_frag = uv;
    vec3 frag_unprojected_pos = (world * vec4(position, 1.0f)).rgb;
    gl_Position = proj * view * vec4(frag_unprojected_pos, 1.0f);

}
