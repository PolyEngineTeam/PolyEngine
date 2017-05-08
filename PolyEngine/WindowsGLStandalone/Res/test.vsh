#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoord;

//uniform mat4 uMVP;
uniform mat4 uTransform;
out vec2 vTexCoord;

void main(){
  gl_Position = uTransform * aPos;
  vTexCoord = aTexCoord;
}