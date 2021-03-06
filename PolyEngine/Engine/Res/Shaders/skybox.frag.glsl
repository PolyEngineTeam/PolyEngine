#version 330 core

in vec3 vUV;

uniform samplerCube uCubemap;
uniform vec4 uTint;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

void main()
{
	color = vec4(uTint.rgb * texture(uCubemap, vUV).rgb, 1.0);
	normal = vec4(0.0);
}