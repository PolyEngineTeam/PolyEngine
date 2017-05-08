#version 330 core
layout(location = 0) in vec4 vertexPosition_modelspace;

//uniform mat4 uMVP;
uniform mat4 uTransform;

void main(){
  gl_Position = uTransform * vertexPosition_modelspace;
}