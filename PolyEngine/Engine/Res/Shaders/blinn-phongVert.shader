#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uMVPTransform;

out vec3 vVertexPos;
out vec2 vTexCoord;
out vec3 vNormal;

void main(){
  gl_Position = uMVPTransform * aPos;
  vTexCoord = aTexCoord;
  vNormal = aNormal;
  vVertexPos = aPos.xyz;
}