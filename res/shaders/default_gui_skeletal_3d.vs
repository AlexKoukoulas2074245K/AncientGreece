#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

uniform mat4 world;
uniform mat4 proj;
uniform mat4 bones[100];

out vec2 uv_frag;

void main()
{
	mat4 boneTransform = bones[boneIds[0]] * weights[0];
	boneTransform += bones[boneIds[1]] * weights[1];
	boneTransform += bones[boneIds[2]] * weights[2];
	boneTransform += bones[boneIds[3]] * weights[3];

    uv_frag = uv;
    vec3 frag_unprojected_pos = (world * boneTransform * vec4(position, 1.0f)).rgb;
    gl_Position = proj * vec4(frag_unprojected_pos, 1.0f);
}
