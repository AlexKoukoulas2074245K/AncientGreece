#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 light_space_matrix;
uniform mat4 world;

void main()
{
    gl_Position = light_space_matrix * world * vec4(position, 1.0);
}
