#version 330 core

in vec2 uv_frag;

out vec4 frag_color;

uniform sampler2D tex;
uniform vec4 custom_color;

void main()
{
	// Calculate final uvs
    float finalUvX = uv_frag.x;    
    float finalUvY = 1.00 - uv_frag.y;    

	// Get texture color
    frag_color = texture(tex, vec2(finalUvX, finalUvY));
	
	// Apply custom color if any
	frag_color.r = min(1.0f, frag_color.r + custom_color.r);
	frag_color.g = min(1.0f, frag_color.g + custom_color.g);
	frag_color.b = min(1.0f, frag_color.b + custom_color.b);		
}
