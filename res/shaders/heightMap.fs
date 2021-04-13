#version 330 core

uniform sampler2D heightMap_texture_0;
uniform sampler2D heightMap_texture_1;
uniform sampler2D heightMap_texture_2;
uniform sampler2D heightMap_texture_3;
uniform sampler2D heightMap_texture_4;
uniform sampler2D shadowMap_texture;
uniform vec3 light_positions[32];
uniform float light_powers[32];
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec3 eye_pos;
uniform float material_shininess;
uniform float dt_accumulator;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;
uniform bool shadows_enabled;
uniform bool is_affected_by_light;

in vec2 uv_frag;
in vec3 normal_interp;
in vec3 frag_pos;
in vec3 frag_unprojected_pos;
in vec4 frag_pos_in_light_space;
in float height_map_scale_factor;

out vec4 frag_color;

#include "include/light_common.fs"

float CalculateShadow()
{
	// perform perspective divide
    vec3 projected_coords = frag_pos_in_light_space.xyz / frag_pos_in_light_space.w;

    // transform to [0,1] range
    projected_coords = projected_coords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap_texture, projected_coords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projected_coords.z;

    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    // calculate shadow factor 
    const float SHADOW_CUTOFF = 0.3f;
	vec3 light_direction = normalize(light_positions[0]);
	float light_normal_dot = dot(vec3(0.0f, 0.0f, -1.0f), light_direction);
	float shadow_factor = 0.0f;
	if (light_normal_dot > SHADOW_CUTOFF) shadow_factor = 1.0f;
	else if (light_normal_dot > 0.0f) shadow_factor = light_normal_dot/SHADOW_CUTOFF;

    return shadow * shadow_factor;
}

void main()
{
	// Calculate final uvs
    float final_uv_x = uv_frag.x;
    if (flip_tex_hor) final_uv_x = 1.00f - final_uv_x;

    float final_uv_y = 1.00f - uv_frag.y;
    if (flip_tex_ver) final_uv_y = 1.00f - final_uv_y;
	
	// Get texture color
	float scale = height_map_scale_factor;
    vec4 tex_color = vec4(0.0);

    const float water_range = 0.00001f;
    const float water_sand_range = 0.01f;
    const float sand_range = 0.02f;
    const float sand_grass_range = 0.05f;
    const float grass_range = 0.10f;
    const float grass_rock_range = 0.25f;
    const float rock_range = 0.35f;
    const float rock_snow_range = 0.55f;
    const float snow_range = 0.75f;

    float waterUvDisplacement = dt_accumulator/50.0f;
    if(scale >= 0.0 && scale <= water_range)
    {
    	tex_color = texture(heightMap_texture_0, vec2(final_uv_x + waterUvDisplacement, final_uv_y + waterUvDisplacement));
    }
	else if(scale <= water_sand_range)
	{
		scale -= water_range;
		scale /= (water_sand_range-water_range);
		
		float scale2 = scale;
		scale = 1.0-scale; 
		
		tex_color += texture(heightMap_texture_0, vec2(final_uv_x + waterUvDisplacement, final_uv_y + waterUvDisplacement))*scale;
		tex_color += texture(heightMap_texture_1, vec2(final_uv_x, final_uv_y))*scale2;
	}
	else if(scale <= sand_range)
	{
		tex_color = texture(heightMap_texture_1, vec2(final_uv_x, final_uv_y));
	}
	else if(scale <= sand_grass_range)
	{
		scale -= sand_range;
		scale /= (sand_grass_range-sand_range);
		
		float scale2 = scale;
		scale = 1.0-scale; 
		
		tex_color += texture(heightMap_texture_1, vec2(final_uv_x, final_uv_y))*scale;
		tex_color += texture(heightMap_texture_2, vec2(final_uv_x, final_uv_y))*scale2;		
	}
	else if(scale <= grass_range)
	{
		tex_color = texture(heightMap_texture_2, vec2(final_uv_x, final_uv_y));
	}
	else if(scale <= grass_rock_range)
	{
		scale -= grass_range;
		scale /= (grass_rock_range-grass_range);
		
		float scale2 = scale;
		scale = 1.0-scale; 
		
		tex_color += texture(heightMap_texture_2, vec2(final_uv_x, final_uv_y))*scale;
		tex_color += texture(heightMap_texture_3, vec2(final_uv_x, final_uv_y))*scale2;		
	}
	else if(scale <= rock_range)
	{
		tex_color = texture(heightMap_texture_3, vec2(final_uv_x, final_uv_y));
	}
	else if(scale <= rock_snow_range)
	{
		scale -= rock_range;
		scale /= (rock_snow_range-rock_range);
		
		float scale2 = scale;
		scale = 1.0-scale; 
		
		tex_color += texture(heightMap_texture_3, vec2(final_uv_x, final_uv_y))*scale;
		tex_color += texture(heightMap_texture_4, vec2(final_uv_x, final_uv_y))*scale2;		
	}
	else
	{
		tex_color = texture(heightMap_texture_4, vec2(final_uv_x, final_uv_y));
	}

	frag_color = tex_color;

	if (is_affected_by_light)
	{ 
		vec3 normal = normalize(normal_interp);

		vec4 diffuse_specular_component = CalculateDiffuseSpecularComponent(normal, 2);
		vec4 ambient_component = CalculateAmbientComponent(normal, 0);
		float shadow = shadows_enabled ? CalculateShadow() : 0.0f;
		frag_color = frag_color * ambient_component + (1.0 - shadow) * diffuse_specular_component;
	}	

	//frag_color = vec4(normal.xyz, 1.0f);

}
