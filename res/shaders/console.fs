#version 330 core

uniform float opaqueness;

in vec2 uv_frag;

out vec4 frag_color;

void main()
{
	// Directly set console color
    frag_color = vec4(0.0, 0.0, 0.0, opaqueness);	
}
