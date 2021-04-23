#version 330 core

in float frag_lifetime;
in vec2 uv_frag;

out vec4 frag_color;

uniform sampler2D tex;

void main()
{
	// Calculate final uvs
    float finalUvX = uv_frag.x;    
    float finalUvY = 1.00 - uv_frag.y;    

	// Get texture color
    frag_color = texture(tex, vec2(finalUvX, finalUvY));
    frag_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
