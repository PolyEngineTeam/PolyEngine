#version 330 core

uniform sampler2D i_color;

in vec2 vTexCoord;
out vec4 color;

void main()
{
	color = vec4(pow(texture(i_color, vTexCoord).rgb, vec3(1.0/2.2)), 1.0);
}