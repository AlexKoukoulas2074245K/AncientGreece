vec4 CalculateDiffuseSpecularComponent(vec3 normal, int nLights) 
{
	// Calculate view direction
	vec3 view_direction = normalize(eye_pos - frag_pos);

 	vec4 light_accumulator = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < nLights; ++i)
	{
		vec3 light_direction = normalize(light_positions[i] - frag_pos);
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

	return light_accumulator;
}

vec4 CalculateAmbientComponent(vec3 normal, int primaryDirectionalLightIndex)
{
	// Diffuse factor
	vec3 light_direction = normalize(light_positions[primaryDirectionalLightIndex] - frag_pos);
	float diffuse_factor = max(dot(normal, light_direction), 0.0f);

	// Diffuse half light ambient component
	vec4 light_ambient = vec4(material_ambient.rgb * diffuse_factor * 0.5f, 1.0f);

	// Constant half ambient component
	vec4 material_ambient = vec4(material_ambient.rgb * 0.5f, 1.0f);
	
	return material_ambient + light_ambient;
}
