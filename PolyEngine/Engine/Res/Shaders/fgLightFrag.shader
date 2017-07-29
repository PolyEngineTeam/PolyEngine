#version 330 core

uniform sampler2D i_color;

in vec2 vTexCoord;
out vec4 color;

void main()
{
	color = texture(i_color, vTexCoord);
}