#version 330 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uClipFromModel;
uniform mat4 uWorldFromModel;
uniform vec4 uViewPosition;

out vec3 vVertexPos;
out vec2 vTexCoord;
out vec3 vNormal;

void main(){
	vec4 vertexInModel = vec4(aPosition.xyz, 1.0);
	gl_Position = uClipFromModel * vertexInModel;
	
	vTexCoord = aTexCoord;
	vNormal = aNormal;
	vVertexPos = aPosition.xyz;
}