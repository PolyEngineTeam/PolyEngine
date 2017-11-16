#version 330 core

in vec3 vVertexPos;
in vec2 vTexCoord;
in vec3 vNormal;

layout(location = 0) out vec4 color;

void main() {
	color.xyz = 0.5 + 0.5 * vNormal;
	color.w = 1.0;
}