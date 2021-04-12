#version 330 core

uniform sampler2D heightMap_texture_0;
uniform sampler2D heightMap_texture_1;
uniform sampler2D heightMap_texture_2;
uniform sampler2D heightMap_texture_3;
uniform sampler2D heightMap_texture_4;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;
uniform int is_affected_by_light;
uniform vec3 light_positions[32];
uniform float light_powers[32];
uniform float dt_accumulator;
uniform vec3 eye_pos;


in vec2 uv_frag;
in vec3 normal_interp;
in vec3 frag_pos;
in vec3 frag_unprojected_pos;
in float height_map_scale_factor;
out vec4 frag_color;

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

    // Normalize normal 
	vec3 normal = normalize(normal_interp);

	// Calculate view direction
	vec3 view_direction = normalize(eye_pos - frag_pos);

	vec4 light_accumulator = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 32; ++i)
	{
		vec3 light_direction = normalize(light_positions[i]);
		vec4 diffuse_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		vec4 specular_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		float diffuse_factor = max(dot(normal, light_direction), 0.0f);
		if (diffuse_factor > 0.0f)
		{
			diffuse_color = material_diffuse * diffuse_factor;
			diffuse_color = clamp(diffuse_color, 0.0f, 1.0f);	

			vec3 reflected_direction = normalize(reflect(-light_direction, normal));

			specular_color = material_specular * pow(max(dot(view_direction, reflected_direction), 0.0f), material_shininess);
			specular_color = clamp(specular_color, 0.0f, 1.0f);
		}
		
		float distance = distance(light_positions[i], frag_unprojected_pos);
		float attenuation = light_powers[i] / (distance * distance);
		
		light_accumulator.rgb += (diffuse_color * attenuation + specular_color * attenuation).rgb;
	}

	frag_color = tex_color;

	if (is_affected_by_light == 1)
	{
		vec3 light_direction = normalize(light_positions[0]);
		float diffuse_factor = max(dot(vec3(0.0f, 0.0f, -1.0f), light_direction), 0.0f);
		vec4 light_ambient = vec4(material_ambient.rgb * diffuse_factor * 0.5f, 1.0f);
		frag_color = frag_color * (vec4(material_ambient.rgb * 0.5f, 1.0f) + light_ambient) + light_accumulator;
	}	

	//frag_color = vec4(normal.xyz, 1.0f);

}
