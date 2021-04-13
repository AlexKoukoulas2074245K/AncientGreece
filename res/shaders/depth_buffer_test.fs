#version 330 core

in vec2 uv_frag;

out vec4 frag_color;

uniform sampler2D depth_map_texture;

void main()
{
	// Calculate final uvs
    float finalUvX = uv_frag.x;    
    float finalUvY = 1.00 - uv_frag.y;    

	// Get texture color
    frag_color = texture(depth_map_texture, vec2(finalUvX, finalUvY));
}
