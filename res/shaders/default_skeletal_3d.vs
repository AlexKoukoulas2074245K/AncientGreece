#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

uniform mat4 norm;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 bones[100];

out vec2 uv_frag;
out vec3 normal_interp;
out vec3 frag_pos;
out vec3 frag_unprojected_pos;

void main()
{
	mat4 boneTransform = bones[boneIds[0]] * weights[0];
	boneTransform += bones[boneIds[1]] * weights[1];
	boneTransform += bones[boneIds[2]] * weights[2];
	boneTransform += bones[boneIds[3]] * weights[3];

    uv_frag = uv;
    frag_unprojected_pos = (boneTransform * world * vec4(position, 1.0f)).rgb;
    normal_interp = (boneTransform * norm * vec4(normal, 0.0f)).rgb;
    gl_Position = proj * view * vec4(frag_unprojected_pos, 1.0f);
    frag_pos = gl_Position.rgb;

}
