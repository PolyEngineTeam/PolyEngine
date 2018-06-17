#version 330 core

uniform sampler2D uTexture;

uniform vec4 uColor;

in vec3 vVertexPos;
in vec2 vTexCoord;

layout(location = 0) out vec4 color;

void main() {
	vec4 texDiffuse = texture(uTexture, vTexCoord);
	color = texDiffuse;
}