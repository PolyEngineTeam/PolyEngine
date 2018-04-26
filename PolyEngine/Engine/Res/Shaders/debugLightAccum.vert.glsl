#version 430 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uClipFromModel;

out vec3 vVertexPos;
out vec2 vTexCoord;

void main() {
    gl_Position = uClipFromModel * aPos;
	vTexCoord = aTexCoord;
	vVertexPos = aPos.xyz;
}