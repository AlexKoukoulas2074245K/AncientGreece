#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 position;
layout(location = 3) in float lifetime;

uniform mat4 view;
uniform mat4 proj;
uniform float particle_size;

out float frag_lifetime;
out vec2 uv_frag;

void main()
{
    uv_frag = uv;
    
    vec3 frag_unprojected_pos = vec3
    (
    	(vertex_position.x * particle_size) + position.x,
    	(vertex_position.y * particle_size) + position.y,
    	(vertex_position.z * particle_size) + position.z
    );

    
    mat4 particle_view_mat = view;
    particle_view_mat[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    particle_view_mat[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    particle_view_mat[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    
    //mat4 particle_view_mat = view;

    frag_lifetime = lifetime;
    gl_Position = proj * particle_view_mat * vec4(frag_unprojected_pos, 1.0f);

}
