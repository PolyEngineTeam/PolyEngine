#version 330 core

in vec2 vTexCoord;
uniform sampler2D i_color;
out vec4 color;

void main()
{
	color = texture(i_color, vTexCoord) + vec4(0.3, 0.4, 1.0, 1.0);
}
