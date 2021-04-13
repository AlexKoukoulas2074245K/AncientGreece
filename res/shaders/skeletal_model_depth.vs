#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

uniform mat4 world;
uniform mat4 light_space_matrix;
uniform mat4 bones[100];

void main()
{
	mat4 boneTransform = bones[boneIds[0]] * weights[0];
	boneTransform += bones[boneIds[1]] * weights[1];
	boneTransform += bones[boneIds[2]] * weights[2];
	boneTransform += bones[boneIds[3]] * weights[3];

    gl_Position = light_space_matrix * world * boneTransform * vec4(position, 1.0);
}
