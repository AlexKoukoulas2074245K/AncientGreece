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
    //frag_color.a = frag_color.r;
    //frag_color = vec4(min(1.0f, frag_lifetime), 0.0f, 0.0f, min(1.0f, frag_lifetime));
}
