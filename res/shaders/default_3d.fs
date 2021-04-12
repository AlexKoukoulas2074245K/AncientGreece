#version 330 core

uniform sampler2D tex;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;
uniform int is_affected_by_light;
uniform vec3 light_positions[32];
uniform float light_powers[32];
uniform vec3 eye_pos;

in vec2 uv_frag;
in vec3 normal_interp;
in vec3 frag_pos;
in vec3 frag_unprojected_pos;

out vec4 frag_color;

#include "include/light_common.fs"

void main()
{
	// Calculate final uvs
    float final_uv_x = uv_frag.x;
    if (flip_tex_hor) final_uv_x = 1.00f - final_uv_x;

    float final_uv_y = 1.00f - uv_frag.y;
    if (flip_tex_ver) final_uv_y = 1.00f - final_uv_y;
	
	// Get texture color
    vec4 tex_color = texture(tex, vec2(final_uv_x, final_uv_y));

    
	frag_color = tex_color;

	if (is_affected_by_light == 1)
	{ 
		vec3 normal = normalize(normal_interp);

		vec4 diffuse_specular_component = CalculateDiffuseSpecularComponent(normal, 2);
		vec4 ambient_component = CalculateAmbientComponent(normal, 0);
		
		frag_color = frag_color * ambient_component + diffuse_specular_component;
	}
}
